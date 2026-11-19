"""Run the running-reference slice once per 12.5 ms tick, retaining both states."""
import argparse
import ctypes
from pathlib import Path
import shutil
import subprocess
import tempfile

from m68hc11 import HC11
from rom_coverage import report
from rom_replays import purge_cycle, transmission_replays, mode4_commands, injector_replays, idle_speed_cycle, factory_cycle, decel_fuel_cycle, passby_cycle, warm_fuel_cycle, follower_motion, diagnostic_transitions, segments, slices, serial, reset_vector, attach_spi


class BuaCPU(HC11):
    """Correct FDIV overflow in the pinned emulator (M68HC11RM, FDIV)."""

    visited = None

    def step(self):
        if self.visited is not None:
            self.visited.add(self.pc)
        overflow = self.mem[self.pc] == 0x03 and self.d >= self.x
        instruction = super().step()
        if overflow:
            self.x = 0xFFFF
            self.z = False
        return instruction

ROOT = Path(__file__).resolve().parents[1]
ROM = ROOT / "evidence/firmware/bua-hac.bin"


def compare(lib):
    cpu = BuaCPU()
    cpu.load(ROM.read_bytes(), 0xC000)
    lib.probe_reset()
    lib.probe_ram.restype = ctypes.POINTER(ctypes.c_ubyte)
    lib.probe_mpu.restype = ctypes.POINTER(ctypes.c_ubyte)
    ram, mpu = lib.probe_ram(), lib.probe_mpu()
    for a, b in [(445, 735), (445, 288), (445, 295), (128, 1), (65535, 65535)]:
        assert cpu.call(0xF2F0, d=a, x=b).d == lib.probe_product(a, b)
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


def compare_error43(lib):
    cpu = BuaCPU()
    cpu.load(ROM.read_bytes(), 0xC000)
    lib.probe_reset()
    ram, mpu = lib.probe_ram(), lib.probe_mpu()
    # Cross both modulo-39 boundaries, including a wrapping counter sample,
    # and exercise the start and finish gates on separate invocations.
    for call in range(1, 79):
        sample = 235 if call <= 39 else 213
        for address, value in ((0, 26 if call in (1, 41) else 10),
                               (0x63, 170), (0x5B, 174)):
            cpu.mem[address] = ram[address] = value
        cpu.mem[0x3FCA] = mpu[10] = sample
        cpu.pc = 0xEB5B
        assert cpu.run(until_pc=0xEBB3, max_steps=80) == "until_pc"
        lib.probe_error43()
        for address in (2, 0x3B, 0xA3, 0xA4):
            assert ram[address] == cpu.mem[address], (call, hex(address))


def compare_irq(lib, drive=False, check_outputs=False, trace_tick=None, road=False, mode4_timeout=False, stopped_airflow=False):
    """Diagnostic full scheduler comparison, reporting detail on the first unequal tick."""
    cpu = BuaCPU()
    cpu.load(ROM.read_bytes(), 0xC000)
    cpu.on_read(0x4008, 0x4008, lambda addr: 1)  # timer IRQ
    lib.probe_irq_setup()
    lib.probe_ram.restype = lib.probe_mpu.restype = ctypes.POINTER(ctypes.c_ubyte)
    ram = lib.probe_ram()
    cpu.load(bytes(ram[:0x500]), 0)
    cpu.load(bytes(lib.probe_mpu()[:64]), 0x3FC0)
    lib.probe_io.restype = ctypes.POINTER(ctypes.c_ubyte)
    cpu.load(bytes(lib.probe_io()[:16]), 0x4000)
    attach_spi(cpu, lib)
    if stopped_airflow:
        lib.probe_stopped_airflow(240, 180)
        for address, value in ((0xB0, 95), (0x7F, 180), (0x82, 255)):
            cpu.mem[address] = ram[address] = value
    if mode4_timeout:
        for address, value in ((0, 157), (0x171, 30), (0x46, 8), (0x151, 4),
                               (0x154, 0x80), (0x3F, 1), (0xB4, 7)):
            cpu.mem[address] = ram[address] = value
    writes = []
    def log_write(addr, value):
        writes.append((addr, value))
        cpu.mem[addr] = value
    cpu.on_write(0x3FC0, 0x3FFF, log_write)
    ram_writes = []
    writer_pc = None
    def trace_step():
        nonlocal writer_pc
        writer_pc = cpu.pc
        return BuaCPU.step(cpu)
    def trace_ram(addr, value):
        if tick == trace_tick:
            ram_writes.append((writer_pc, addr, cpu.mem[addr], value))
        cpu.mem[addr] = value
    failed = 0
    output_failures = 0
    state_failures = 0
    running = [None, None]
    injection_count = 0
    for tick in range(1, 513 if road else 257 if drive else 33):
        if tick == trace_tick:
            cpu.step = trace_step
            cpu.on_write(0, 0x4FF, trace_ram)
        writes.clear()
        # The two serial cell interrupts return to the wait loop before the
        # next minor-loop interrupt. Execute their ROM bodies when armed.
        for mask in (1, 2):
            if cpu.mem[0x46] & mask:
                cpu.pc, cpu.sp = cpu.read16(0xFFF2), 0x1F6
                cpu.write16(0x1FE, 0xCCC6)
                if cpu.run(until_pc=0xCCC6, max_steps=20000) != "until_pc":
                    raise SystemExit(f"serial IRQ stopped at ${cpu.pc:04X}")
        if drive:
            # Both implementations receive the same peripheral stimulus. Only
            # host-generated capture/events cross over, never computed ECM RAM.
            lib.probe_rpm(0 if tick <= 32 else (300 if tick <= 96 else 1100))
            cpu.mem[0x3FF8:0x3FFA] = (2000).to_bytes(2, "big")  # valid VATS period
            mpu = lib.probe_mpu()
            mpu[0x38], mpu[0x39] = divmod(2000, 256)
        if road:
            lib.probe_road(40 if tick >= 160 else 0)
        lib.probe_irq()
        if drive:
            if road:
                cpu.mem[0x3FC2:0x3FC4] = lib.probe_vss_capture().to_bytes(2, "big")
            injections = lib.probe_injections()
            cpu.mem[0x3FFA] = 8 * lib.probe_reference_seen() | (64 if injections != injection_count else 0)
            injection_count = injections
            if lib.probe_reference_seen():
                cpu.mem[0x3FC0:0x3FC2] = lib.probe_capture().to_bytes(2, "big")
        cpu.pc, cpu.sp = cpu.read16(0xFFF2), 0x1F6
        reason = cpu.run(until_pc=0xCCC6, max_steps=20000)
        if reason != "until_pc":
            raise SystemExit(f"IRQ {tick}: ROM stopped at ${cpu.pc:04X} ({reason})")
        if stopped_airflow and tick <= 3:
            assert cpu.mem[0xB0] == min(95 + tick, 97), (tick, "MAF timer")
            assert bool(cpu.mem[0x3F] & 128) == (tick == 3), (tick, "high MAF")
        if mode4_timeout and tick <= 4:
            assert bool(cpu.mem[0x35] & 8) == (tick <= 3), (tick, "Mode 4 latch")
            assert cpu.mem[0x151] == (4 if tick < 3 else 0), (tick, "SCI mode")
        for i, flags in enumerate((cpu.mem[0x34], ram[0x34])):
            if flags & 128 and running[i] is None:
                running[i] = tick
        diffs = [(i, cpu.mem[i], ram[i]) for i in range(0x500)
                 if not 0x1C0 <= i <= 0x1FF and cpu.mem[i] != ram[i]]
        if tick == trace_tick:
            differing = {addr for addr, _, _ in diffs}
            for pc, addr, old, new in ram_writes:
                if addr in differing:
                    print(f"${pc:04X} wrote ${addr:04X}: {old:02X} -> {new:02X}")
            print(f"Tick {tick}: {len(differing)} non-stack RAM differences")
        actual = [(lib.probe_address(i), lib.probe_value(i))
                  for i in range(lib.probe_trace_size())]
        if writes != actual:
            output_failures += 1
            if output_failures == 1:
                print(f"First unequal MPU writes at IRQ {tick}: ROM={writes}, C={actual}")
        # Monitor the specific IRQ state covered by the output regression.
        addresses = (0x46, 0xC2, 0xC3, 0xD9, 0xDA)
        if any(cpu.mem[a] != ram[a] for a in addresses) or (cpu.mem[0x30] ^ ram[0x30]) & 0x80:
            state_failures += 1
        if diffs or writes != actual:
            if check_outputs:
                continue
            print(f"IRQ {tick}: {len(diffs)} RAM differences, "
                  f"{len(writes)} ROM and {len(actual)} C MPU byte writes")
            failed += 1
            if failed > 1:
                continue
            for addr, rom, model in diffs[:12]:
                print(f"  ${addr:04X} ROM={rom:02X} C={model:02X}")
            if writes != actual:
                print(f"  MPU writes ROM={writes}, C={actual}")
    if not check_outputs:
        print(f"{failed} of {tick} full IRQ ticks differed")
    print(f"Ordered MPU writes matched on {tick - output_failures}/{tick} ticks")
    if drive:
        print(f"First running tick ROM={running[0]}, C={running[1]}")
    print(f"Fuel state, FMD selector and serial flags matched on {tick - state_failures}/{tick} ticks")
    if drive and (None in running or running[0] != running[1]):
        raise SystemExit("engine running transition differed or was never observed")
    failures = (output_failures + state_failures) if check_outputs else failed
    if failures:
        raise SystemExit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--full-irq", action="store_true")
    group.add_argument("--road-irq", action="store_true")
    group.add_argument("--drive-irq", action="store_true")
    group.add_argument("--all", action="store_true", help="run all scenarios and combine their coverage")
    parser.add_argument("--check-outputs", action="store_true",
                        help="check ordered MPU writes and fuel/FMD/serial state")
    parser.add_argument("--trace-tick", type=int, help="show ROM stores to differing RAM bytes")
    parser.add_argument("--coverage", action="store_true", help="report unique executed instruction starts by listing region")
    group.add_argument("--faults", action="store_true", help="replay diagnostic qualification transitions")
    group.add_argument("--segments", action="store_true")
    group.add_argument("--reset-vector", action="store_true")
    args = parser.parse_args()
    if args.coverage:
        BuaCPU.visited = set()
    if args.trace_tick is not None and not (
        (args.road_irq or args.drive_irq or args.full_irq) and 1 <= args.trace_tick <= (512 if args.road_irq else 256 if args.drive_irq else 32)
    ):
        parser.error("--trace-tick requires a tick in the selected IRQ scenario")
    if args.check_outputs and not (args.full_irq or args.drive_irq):
        parser.error("--check-outputs requires an IRQ scenario")
    with tempfile.TemporaryDirectory() as temp:
        bridge = Path(temp) / "probe.so"
        subprocess.run(["cc", "-std=c89", "-shared", "-fPIC",
                        str(ROOT / "tests/spark_rom_probe.c"), "-o", str(bridge)], check=True)
        scenarios = {
            "spark": lambda lib: (compare(lib), compare_error43(lib)),
            "stopped": lambda lib: compare_irq(lib, False, args.check_outputs, args.trace_tick),
            "drive": lambda lib: compare_irq(lib, True, args.check_outputs, args.trace_tick),
            "road": lambda lib: compare_irq(lib, True, args.check_outputs, args.trace_tick, road=True),
            "mode4_timeout": lambda lib: compare_irq(lib, mode4_timeout=True),
            "stopped_airflow": lambda lib: compare_irq(lib, stopped_airflow=True),
            "reset": lambda lib: reset_vector(BuaCPU(), ROM.read_bytes(), lib),
            "faults": lambda lib: diagnostic_transitions(BuaCPU, ROM.read_bytes(), lib),
            "targeted": lambda lib: (segments(BuaCPU, ROM.read_bytes(), lib),
                                     purge_cycle(BuaCPU(), ROM.read_bytes(), lib),
                                     transmission_replays(BuaCPU, ROM.read_bytes(), lib),
                                     mode4_commands(BuaCPU, ROM.read_bytes(), lib),
                                     injector_replays(BuaCPU, ROM.read_bytes(), lib),
                                     factory_cycle(BuaCPU(), ROM.read_bytes(), lib),
                                     passby_cycle(BuaCPU(), ROM.read_bytes(), lib),
                                     decel_fuel_cycle(BuaCPU(), ROM.read_bytes(), lib),
                                     warm_fuel_cycle(BuaCPU(), ROM.read_bytes(), lib),
                                     idle_speed_cycle(BuaCPU(), ROM.read_bytes(), lib),
                                     follower_motion(BuaCPU(), ROM.read_bytes(), lib),
                                     slices(BuaCPU, ROM.read_bytes(), lib),
                                     serial(BuaCPU, ROM.read_bytes(), lib)),
        }
        selected = ("faults" if args.faults else "reset" if args.reset_vector else "targeted" if args.segments
                    else "road" if args.road_irq else "drive" if args.drive_irq else "stopped" if args.full_irq else "spark")
        for name in scenarios if args.all else [selected]:
            # A separate loaded image gives every scenario fresh C static state.
            path = Path(temp) / (name + ".so")
            shutil.copyfile(bridge, path)
            before = len(BuaCPU.visited) if args.coverage else 0
            scenarios[name](ctypes.CDLL(str(path)))
            if args.coverage:
                print(f"{name}: +{len(BuaCPU.visited)-before} unique PCs, {len(BuaCPU.visited)} cumulative")
    if args.coverage:
        report(ROM.with_suffix(".lst"), ROM.read_bytes(), BuaCPU.visited)
