"""Run the running-reference slice once per 12.5 ms tick, retaining both states."""
import argparse
import ctypes
from pathlib import Path
import subprocess
import tempfile

from m68hc11 import HC11

ROOT = Path(__file__).resolve().parents[1]
ROM = ROOT / "evidence/firmware/bua-hac.bin"


def compare(lib):
    cpu = HC11()
    cpu.load(ROM.read_bytes(), 0xC000)
    lib.probe_reset()
    lib.probe_ram.restype = ctypes.POINTER(ctypes.c_ubyte)
    lib.probe_mpu.restype = ctypes.POINTER(ctypes.c_ubyte)
    ram, mpu = lib.probe_ram(), lib.probe_mpu()
    for old in [0, 0x1234, 0xFFFF]:
        for value in [0, 123, 255]:
            for coef in [0, 1, 127, 128, 255]:
                cpu.a, cpu.b, cpu.x = value, coef, old
                result = cpu.call(0xF250)
                assert result.d == lib.probe_filter(old, value, coef)
    for address, value in [(0x34, 128), (0x5B, 140), (0x63, 86), (0x7E, 126)]:
        cpu.mem[address] = ram[address] = value
    cpu.on_read(0x4001, 0x4001, lambda addr: 128)
    fmd_reads = []
    def fmd_reply(addr):
        if cpu.mem[0x4002] & 4:
            fmd_reads.append(cpu.mem[0x4000])
        return 0
    cpu.on_read(0x4000, 0x4000, fmd_reply)
    writes = []
    def log_write(addr, value):
        writes.append((addr, value))
        cpu.mem[addr] = value
    cpu.on_write(0x3FC0, 0x3FFF, log_write)
    for tick, period in enumerate([1800, 1600, 1400, 1200, 900, 750, 1200, 1800, 6000, 8000, 9000, 9000, 800, 600, 0x8400, 0xC000], 1):
        mode4 = 8 if tick in (5, 6, 7, 8) else 0
        cpu.write16(0x3FCA, tick * 70)
        mpu[10], mpu[11] = divmod(tick * 70, 256)
        for address, value in [(0x158, {6: 24, 7: 40, 8: 56}.get(tick, 8)), (0x15B, 30), (0x154, mode4), (0x95, period >> 8), (0x96, period & 255), (0x35, 4 | mode4)]:
            cpu.mem[address] = ram[address] = value
        writes.clear()
        fmd_reads.clear()
        cpu.pc, cpu.sp = 0xCD4B, 0x1FF
        reason = cpu.run(until_pc=0xD36A, max_steps=5000)
        assert reason == "until_pc", (reason, hex(cpu.pc))
        lib.probe_tick()
        diffs = [(hex(i), cpu.mem[i], ram[i]) for i in range(0x500)
                 if not 0x1C0 <= i <= 0x1FF and cpu.mem[i] != ram[i]]
        outputs = [(hex(0x3FC0+i), cpu.mem[0x3FC0+i], mpu[i])
                   for i in range(64) if cpu.mem[0x3FC0+i] != mpu[i]]
        lib.probe_trace_size.restype = ctypes.c_uint
        expected = [(lib.probe_address(i), lib.probe_value(i))
                    for i in range(lib.probe_trace_size())]
        assert len(fmd_reads) == lib.probe_fmd_reads(), (tick, fmd_reads)
        assert writes == expected, (tick, writes, expected)
        assert not diffs and not outputs, (tick, diffs, outputs)
    for diff, ignition_off, expected in [(84, 0, 64), (85, 0, 0), (0, 16, 0)]:
        for address, value in [(0x34, 0), (0x35, 4), (0x33, 8 | ignition_off),
                               (0x95, 2), (0x96, 88), (0x93, 255), (0xEF, 0), (0xF0, diff), (0x3F, 0)]:
            cpu.mem[address] = ram[address] = value
        cpu.pc, cpu.sp = 0xCD4B, 0x1FF
        assert cpu.run(until_pc=0xCDDD, max_steps=100) == "until_pc"
        lib.probe_tick()
        assert (cpu.mem[0x3F] & 64) == (ram[0x3F] & 64) == expected
    ram[0x9F] = cpu.mem[0x9F] = 23
    ram[0x35] = cpu.mem[0x35] = 0
    writes.clear()
    cpu.pc = 0xCD4B
    assert cpu.run(until_pc=0xCDCD, max_steps=100) == "until_pc"
    lib.probe_tick()
    assert writes == [(0x3FC0, 255), (0x3FC1, 255)]
    assert bytes(mpu[:2]) == bytes([255, 255]) and lib.probe_trace_size() == 2
    print(f"{tick} running spark ticks matched RAM and ordered MPU writes")


def compare_irq(lib):
    """Diagnostic full scheduler comparison, reporting detail on the first unequal tick."""
    cpu = HC11()
    cpu.load(ROM.read_bytes(), 0xC000)
    cpu.on_read(0x4008, 0x4008, lambda addr: 1)  # timer IRQ
    cpu.on_read(0x4001, 0x4001, lambda addr: 128)  # SPI complete
    lib.probe_irq_setup()
    lib.probe_ram.restype = lib.probe_mpu.restype = ctypes.POINTER(ctypes.c_ubyte)
    ram = lib.probe_ram()
    cpu.load(bytes(ram[:0x500]), 0)
    cpu.load(bytes(lib.probe_mpu()[:64]), 0x3FC0)
    lib.probe_io.restype = ctypes.POINTER(ctypes.c_ubyte)
    cpu.load(bytes(lib.probe_io()[:16]), 0x4000)
    command, channel = 0, 0
    def spi_write(addr, value):
        nonlocal command, channel
        command = value
        if not cpu.mem[0x4002] & 4 and value != 0xB0:
            channel = value
    def spi_read(addr):
        if cpu.mem[0x4002] & 4:
            return lib.probe_fmd(command)
        return lib.probe_adc(channel)
    cpu.on_write(0x4000, 0x4000, spi_write)
    cpu.on_read(0x4000, 0x4000, spi_read)
    writes = []
    def log_write(addr, value):
        writes.append((addr, value))
        cpu.mem[addr] = value
    cpu.on_write(0x3FC0, 0x3FFF, log_write)
    failed = 0
    for tick in range(1, 33):
        writes.clear()
        # The two serial cell interrupts return to the wait loop before the
        # next minor-loop interrupt. Execute their ROM bodies when armed.
        for mask in (1, 2):
            if cpu.mem[0x46] & mask:
                cpu.pc, cpu.sp = cpu.read16(0xFFF2), 0x1F6
                cpu.write16(0x1FE, 0xCCC6)
                if cpu.run(until_pc=0xCCC6, max_steps=20000) != "until_pc":
                    raise SystemExit(f"serial IRQ stopped at ${cpu.pc:04X}")
        cpu.pc, cpu.sp = cpu.read16(0xFFF2), 0x1F6
        reason = cpu.run(until_pc=0xCCC6, max_steps=20000)
        if reason != "until_pc":
            raise SystemExit(f"IRQ {tick}: ROM stopped at ${cpu.pc:04X} ({reason})")
        lib.probe_irq()
        diffs = [(i, cpu.mem[i], ram[i]) for i in range(0x500)
                 if not 0x1C0 <= i <= 0x1FF and cpu.mem[i] != ram[i]]
        actual = [(lib.probe_address(i), lib.probe_value(i))
                  for i in range(lib.probe_trace_size())]
        if diffs or writes != actual:
            print(f"IRQ {tick}: {len(diffs)} RAM differences, "
                  f"{len(writes)} ROM and {len(actual)} C MPU byte writes")
            failed += 1
            if failed > 1:
                continue
            for addr, rom, model in diffs[:12]:
                print(f"  ${addr:04X} ROM={rom:02X} C={model:02X}")
            if writes != actual:
                print(f"  MPU writes ROM={writes}, C={actual}")
    print(f"{failed} of {tick} full IRQ ticks differed")
    if failed:
        raise SystemExit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--full-irq", action="store_true")
    args = parser.parse_args()
    with tempfile.TemporaryDirectory() as temp:
        bridge = Path(temp) / "probe.so"
        subprocess.run(["cc", "-std=c89", "-shared", "-fPIC",
                        str(ROOT / "tests/spark_rom_probe.c"), "-o", str(bridge)], check=True)
        (compare_irq if args.full_irq else compare)(ctypes.CDLL(str(bridge)))
