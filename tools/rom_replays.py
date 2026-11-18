"""Independent ROM entry replays with common peripheral inputs."""
import ctypes


def attach_spi(cpu, lib):
    command = channel = 0
    def write(addr, value):
        nonlocal command, channel
        command = value
        if not cpu.mem[0x4002] & 4 and value != 0xB0:
            channel = value

    def read(addr):
        return lib.probe_fmd(command) if cpu.mem[0x4002] & 4 else lib.probe_adc(channel)

    cpu.on_read(0x4001, 0x4001, lambda addr: cpu.mem[addr] | 128)
    cpu.on_write(0x4000, 0x4000, write)
    cpu.on_read(0x4000, 0x4000, read)


def hardware(cpu, rom, lib, reset=False):
    for name in ("probe_ram", "probe_mpu", "probe_io"):
        getattr(lib, name).restype = ctypes.POINTER(ctypes.c_ubyte)
    lib.probe_irq_setup()
    lib.probe_temperature_inputs(120, 0)
    ram, mpu, io = lib.probe_ram(), lib.probe_mpu(), lib.probe_io()
    cpu.load(rom, 0xC000)
    if not reset:
        for addr, data, size in ((0, ram, 0x500), (0x3FC0, mpu, 64), (0x4000, io, 16)):
            cpu.load(bytes(data[:size]), addr)
    attach_spi(cpu, lib)
    return ram, mpu, io


def differences(cpu, lib, include_io=False):
    banks = [(0, lib.probe_ram(), 0x500), (0x3FC0, lib.probe_mpu(), 64)]
    if include_io:
        banks.append((0x4000, lib.probe_io(), 16))
    differences_found = []
    for base, data, size in banks:
        for i in range(size):
            addr = base + i
            if 0x1C0 <= addr <= 0x1FF:
                continue
            value = cpu.read8(addr) if addr == 0x4001 else cpu.mem[addr]
            if value != data[i]:
                differences_found.append((addr, value, data[i]))
    return differences_found


def check_state(name, cpu, lib, include_io=False):
    diff = differences(cpu, lib, include_io)
    if diff:
        detail = ", ".join(f"${a:04X} ROM={v:02X} C={c:02X}" for a,v,c in diff)
        raise AssertionError(f"{name}: {detail}")


def reset_vector(cpu, rom, lib):
    hardware(cpu, rom, lib, reset=True)
    cpu.pc = cpu.read16(0xFFFE)
    reason = cpu.run(until_pc=0xCCC6, max_steps=200000)
    if reason != "until_pc":
        raise AssertionError(f"reset ended at ${cpu.pc:04X} ({reason})")
    fixture = differences(cpu, lib, include_io=True)
    print(f"ecm_reset fixture: {len(fixture)} differences versus reset vector")
    lib.probe_cold_boot()
    # SPI completion is a peripheral input, including when reading back
    # the prescaler/status register after LF1F9 clears its start bit.
    check_state("cold reset", cpu, lib, include_io=True)
    assert cpu.mem[0x5000] == lib.probe_map()
    print("Cold reset vector: RAM, MPU and I/O matched at $CCC6")


# Preloads are scenario inputs. Each dispatch compares the rest of RAM as well
# as the complete MPU backing and ordered writes.
SEGMENTS = [
    ("in-gear EGR", 11, {0x37:0, 0x60:150, 0x82:110, 0x57:75, 0x63:110, 0x5B:180}),
    ("warm TCC lock", 14, {4:1,0x37:0,0x34:128,0x57:55,0x65:40,0x67:12,0x5B:160,0x82:70}),
    ("warm TCC unlock", 14, {4:0x81,0x37:0x20,0x34:128,0x57:55,0x65:40,0x67:12,0x5B:160,0x82:240}),
    ("in-gear RPM/VSS", 14, {0x37:0,0x34:128,0x57:76,0x65:40,0x67:12,0x5B:50}),
    ("MAT open-circuit Error 23", 10, {0x1B:241, 0xAC:121}),
    ("MAT short-circuit Error 25", 10, {0x1B:241, 0xAD:121, 0x65:30}),
    ("low CTS A/D fault qualification", 6, {0x1A:1, 0x1B:40, 0x5B:250, 0x3C:128}),
    ("warm running segment 5", 5, {0:0x15,0x34:128,0x01:2,0x1A:1,0x1B:35,0x5B:180,0xC7:250}),
    ("warm purge", 12, {0x1B:200, 0x5B:190, 0x63:120, 0x82:80, 0x57:75}),
    ("running output stage", 1, {0x34:128, 0x112:67, 0x113:95, 0x3E:33}),
    ("diagnostic flash", 13, {0x35:32, 0x48:2, 0x49:0x80}),
    ("running diagnostic faults", 13, {0x34:128, 0x1A:1, 0x4C:0x90, 0x3F:0x44, 0x7E:190}),
]


def ordered_writes(lib):
    return [(lib.probe_address(i), lib.probe_value(i)) for i in range(lib.probe_trace_size())]


def segments(make_cpu, rom, lib):
    for name, segment, inputs in SEGMENTS:
        cpu = make_cpu()
        ram, _, _ = hardware(cpu, rom, lib)
        if segment in (6, 10):
            lib.probe_temperature_inputs(255 if segment == 6 else 120,
                                         255 if name.endswith("23") else 0)
        for addr, value in inputs.items():
            cpu.mem[addr] = ram[addr] = value
        writes = []
        def write(addr, value):
            cpu.mem[addr] = value
            writes.append((addr, value))
        cpu.on_write(0x3FC0, 0x3FFF, write)
        cpu.sp = 0x1FF
        cpu.call(cpu.read16(0xCCE4 + 2 * segment), max_steps=10000)
        assert cpu.pc == 0xFFFE, (name, hex(cpu.pc))
        lib.probe_major(segment)
        check_state(name, cpu, lib)
        if name.startswith("warm TCC"):
            locked = name == "warm TCC lock"
            assert bool(ram[4] & 128) == bool(ram[0x37] & 32) == locked
        if name.endswith("Error 23"):
            assert ram[0x4C] & 1
        if name.endswith("Error 25"):
            assert ram[0x4D] & 0x40
        actual = ordered_writes(lib)
        assert writes == actual, (name, writes, actual)
        print(f"{name}: RAM and MPU matched")

# Each boundary is a ROM PC and its matching scheduler-called C routine.
# Timer preloads select the pass immediately before a calibrated transition.
SLICES = [
    ("PE entry", "afr", 0xD9BC, 0xDA5A, {0x56:160,0x57:80,0x63:220,0x82:250,0x39:0x84}),
    ("warm closed-loop O2 and BLM", "warm_fuel", 0xD86B, 0xCCAC,
     {0:1,0x34:128,0x44:128,0x5B:170,0x63:110,0x1A:1,0x1B:30,
      0xC7:200,0xC4:1,0xC1:160,0x57:60,0x58:120,0x71:100,0x95:4,0x03:32}),
    ("warm stable BLM cell", "warm_fuel", 0xD86B, 0xCCAC,
     {0:1,0x34:128,0x44:0xC2,0x5B:170,0x63:110,0x1A:1,0x1B:30,0xBF:2,
      0xC7:200,0xC4:1,0xC1:160,0x57:60,0x58:120,0x71:100,0x95:4,0x03:32}),
    ("BLM learn threshold", "blm_tail", 0xDDC2, 0xCCAC,
     {0:1,0xBF:3,0x1F:128,0x44:2,0xC9:134,0xCA:27}),
    ("TPS acceleration", "accel", 0xD86B, 0xD9A9, {0x82:150,0xDD:30,0x63:160,0x61:50,0xE7:60}),
    ("key-off housekeeping", "keyoff", 0xD6D1, 0xD76D, {0x33:0x30,0x8C:8,0x34:128,0x1B:120}),
    ("key-off powerdown", "keyoff", 0xD6D1, 0xD6EA, {0x33:16,0x8B:3,0x8C:0x85}),
    ("idle PID underspeed", "homing", 0xD370, 0xD6CE, {0:8,0x34:128,0xF3:0x98,0xF2:4,0x58:30,0x5B:160,0xFF:30}),
    ("IAC homing", "homing", 0xD370, 0xD6CE, {0x33:16,0x2C:80}),
    ("IAC paced close", "motor", 0xF4DF, 0xF67B, {0x34:128,0xF3:8,0x101:0x86,0x104:2}),
    ("flash stored code", "flash", 0xF10E, 0xFFFE, {0x42:0x60,0xBD:1,0xBB:2,6:128}),
    ("Mode 4 reset commands", "mode4", 0xCB5F, 0xCBB4, {0x46:8,0x151:4,0x154:0x70,5:0x34}),
    ("factory output exercise", "factory", 0xFCA2, 0xCCC6, {0x47:128}),
]


# LE87A enters with the IAC enabled. Override the PC fixture's A/C and
# steering inputs explicitly so each load transition has a known origin.
FOLLOWER_CASES = [
    ("fan request below threshold", {0xF4: 1, 0x10E: 9}),
    ("fan anticipation complete", {0xF4: 1, 0x10E: 10}),
    ("cold request below threshold", {0x3B: 64, 0x10E: 9}),
    ("cold anticipation complete", {0x3B: 64, 0x10E: 10}),
    ("combined stall and DFCO offsets", {0x84: 20, 0x3D: 16, 0xC5: 2, 0x43: 2}),
    ("stall offset overflow", {0x84: 250, 0x3D: 16, 0xC5: 1}),
    ("DFCO offset overflow", {0x84: 250, 0x43: 2}),
    ("decel enleanment", {0x84: 35, 0x3E: 128}),
    ("power steering with AC", {0xF2: 1, 0x37: 8, 0x84: 12}),
    ("AC learned offset at maximum", {0xF2: 0, 0x2D: 32, 0x84: 90}),
    ("AC learned offset above maximum", {0xF2: 0, 0x2D: 33, 0x84: 12}),
    ("AC learned offset below minimum", {0xF2: 0, 0x2D: 0, 0x84: 12}),
    ("park throttle follower", {0xF3: 136, 0x84: 90}),
    ("moving drive throttle follower", {0x67: 7, 0x84: 30}),
    ("load removed at target", {0xF2: 193, 0x84: 20, 0x105: 30}),
    ("small closing correction", {0x84: 20, 0x105: 32}),
    ("128-step closing correction", {0x84: 20, 0x105: 158}),
    ("large closing correction", {0x84: 20, 0x105: 240}),
    ("opening at travel limit", {0x84: 20, 0x2C: 145}),
    ("throttle release", {0x84: 2, 0x85: 4, 0x105: 1}),
    ("idle speed above target", {0x58: 60, 0x102: 50}),
    ("idle speed returns to target", {0xF2: 130, 0x58: 50, 0x102: 50}),
    ("Mode 4 idle inhibit", {0x35: 8, 0x154: 2, 0x155: 0}),
    ("Mode 4 idle enable", {0x35: 8, 0x154: 2, 0x155: 2, 0xF2: 132}),
]
for name, overrides in FOLLOWER_CASES:
    inputs = {0xF3: 8, 0xF2: 128, 0x37: 0, 0xF4: 0, 0x3B: 0, 1: 0, 0x34: 0}
    inputs.update(overrides)
    SLICES.append((name, "follower", 0xE87A, 0xFFFE, inputs))


# LDB35 takes filtered samples. The paired fast/slow readings choose the
# rich, lean or disagreement path without sampling a host ADC mid-replay.
FEEDBACK_CASES = [
    ("rising fast O2 in window", {0xEA: 0, 0x6F: 107, 0xC8: 106}),
    ("steady fast O2 in window", {0xEA: 0, 0x6F: 107, 0xC8: 107}),
    ("cold delay saturation", {0x6F: 0, 0x71: 78, 0xEA: 0, 0x5B: 1}),
    ("lean integrator count", {0x6F: 0, 0x71: 0, 0xC4: 0}),
    ("lean integrator step", {0x6F: 0, 0x71: 0}),
    ("rich integrator step", {0x6F: 255, 0x71: 255}),
    ("cold idle delay", {0x6F: 0, 0x71: 0, 0x5B: 1, 0x82: 0}),
    ("moving idle throttle", {0x6F: 0, 0x71: 0, 0x67: 5, 0x82: 0}),
    ("lean maximum integrator", {0x6F: 0, 0x71: 0, 0xC9: 180}),
    ("rich minimum integrator", {0x6F: 255, 0x71: 255, 0xC9: 40}),
    ("lean correction overflow", {0x6F: 0, 0x71: 0, 0xC9: 254}),
    ("rich correction underflow", {0x6F: 255, 0x71: 255, 0xC9: 1}),
    ("persistent opposite samples", {0x6F: 0, 0x71: 140, 0xC7: 254, 0x46C: 29}),
    ("slow O2 deadband", {0x44: 160, 0x6F: 0, 0x71: 105}),
    ("high airflow and RPM", {0xEA: 96, 0x56: 190, 0x71: 255, 0x6F: 255}),
    ("transient fuel inhibit", {0x38: 128}),
    ("deceleration lean reset", {0x63: 40, 0x57: 29, 0x6F: 0}),
    ("deceleration below neutral", {0x3E: 128, 0x6F: 0, 0xC9: 127}),
    ("deceleration rich feedback", {0x3E: 128, 0x6F: 255}),
]
for name, overrides in FEEDBACK_CASES:
    inputs = {0x44: 128, 0x43: 0, 0x3E: 0, 0x63: 80, 0x82: 10, 0xC4: 255,
              0x5B: 170, 0xC9: 128, 0xBE: 30}
    inputs.update(overrides)
    SLICES.append((name, "feedback", 0xDB35, 0xDD29, inputs))

SLICES.extend([
    ("invalid retained BLM", "blm_select", 0xDA73, 0xDB35,
     {0xBF: 0, 0x57: 0, 0xEA: 0, 0x1C: 0, 0x0A: 33, 0x0C: 44}),
    ("PE below-neutral BLM", "blm_select", 0xDA73, 0xDB35,
     {0xBF: 0, 0x57: 0, 0xEA: 0, 0x1C: 120, 0x43: 32}),
    ("high flow BLM cell", "blm_select", 0xDA73, 0xDB35,
     {0xBF: 0, 0x57: 150, 0xEA: 100}),
])


def slices(make_cpu, rom, lib):
    failures = []
    for name, fn, start, end, inputs in SLICES:
        cpu = make_cpu()
        ram, _, _ = hardware(cpu, rom, lib)
        if fn == "factory":
            lib.probe_factory_inputs()
        for addr, value in inputs.items():
            cpu.mem[addr] = ram[addr] = value
        writes = []
        def write(addr, value):
            cpu.mem[addr] = value
            writes.append((addr, value))
        cpu.on_write(0x3FC0, 0x3FFF, write)
        cpu.sp = 0x1FD
        cpu.write16(0x1FE, 0xFFFE)
        cpu.pc = start
        reason = cpu.run(until_pc=end, max_steps=10000)
        if reason != "until_pc":
            failures.append(f"{name}: stopped at ${cpu.pc:04X} ({reason})")
            continue
        result = getattr(lib, "probe_" + fn)()
        try:
            check_state(name, cpu, lib)
            if fn == "feedback":
                assert result == cpu.mem[cpu.sp + 1], (name, "fuel proportional step")
            actual = ordered_writes(lib)
            assert writes == actual, (name, writes, actual)
            if fn == "blm_tail":
                assert ram[0x1F] == 130 and ram[0xC0] == 130
                # Continue for 64 more eligible 50 ms services, keeping all
                # learned state and checking the transition on every pass.
                for n in range(1, 65):
                    cpu.mem[0] = ram[0] = (1 + 8*n) % 160
                    cpu.pc = start
                    writes.clear()
                    assert cpu.run(until_pc=end, max_steps=10000) == "until_pc"
                    lib.probe_blm_tail()
                    check_state(f"BLM service {n+1}", cpu, lib)
                    assert writes == ordered_writes(lib)
            print(f"{name}: RAM and MPU matched")
        except AssertionError as err:
            failures.append(str(err))
    if failures:
        raise AssertionError("\n".join(failures))


def warm_fuel_cycle(cpu, rom, lib):
    ram, _, _ = hardware(cpu, rom, lib)
    inputs = {0x34: 128, 0x44: 0x82, 0x5B: 170, 0x63: 110, 0x1A: 1,
              0x1B: 30, 0xBF: 2, 0x57: 60, 0x58: 120, 0x95: 4, 0xEA: 12, 0xCB: 64, 3: 32}
    for addr, value in inputs.items():
        ram[addr] = cpu.mem[addr] = value
    writes = []

    def write(addr, value):
        cpu.mem[addr] = value
        writes.append((addr, value))

    cpu.on_write(0x3FC0, 0x3FFF, write)
    endpoints = []
    for phase, sample in enumerate((20, 230)):
        lib.probe_o2_input(sample)
        # $71 is the slow O2 sample produced outside this routine. The
        # fast sample comes through the actual ADC read inside the replay.
        ram[0x71] = cpu.mem[0x71] = sample
        for tick in range(64):
            ram[0] = cpu.mem[0] = (1 + 2 * (64 * phase + tick)) % 160
            cpu.pc, cpu.sp = 0xD86B, 0x1FD
            writes.clear()
            assert cpu.run(until_pc=0xCCAC, max_steps=10000) == "until_pc"
            lib.probe_warm_fuel()
            label = f"Warm fuel ADC {sample}, pass {tick + 1}"
            check_state(label, cpu, lib)
            assert writes == ordered_writes(lib), (label, writes, ordered_writes(lib))
            assert [addr for addr, _ in writes if addr in (0x3FD0, 0x3FD1)] == [0x3FD0, 0x3FD1], label
        endpoints.append(ram[0xC9])
    assert endpoints[0] > 128 and endpoints[1] < endpoints[0], endpoints
    print(f"128 warm fuel passes: lean/rich integrator endpoints {endpoints}, RAM and MPU writes matched")


def follower_motion(cpu, rom, lib):
    ram, _, _ = hardware(cpu, rom, lib)
    for addr, value in {0x34: 128, 0x37: 0, 0x7E: 124, 0x2C: 40, 0xF2: 132,
                        0xF3: 8, 0x105: 0, 0x101: 0, 0x104: 1}.items():
        cpu.mem[addr] = ram[addr] = value
    writes = []

    def write(addr, value):
        cpu.mem[addr] = value
        writes.append((addr, value))

    cpu.on_write(0x3FC0, 0x3FFF, write)
    # Seed only the input TPS. Position, outstanding demand and phase counters
    # persist across every motor and request service in each implementation.
    for tick in range(97):
        cpu.sp = 0x1FF
        cpu.pc = 0xF4DF
        writes.clear()
        assert cpu.run(until_pc=0xF67B, max_steps=2000) == "until_pc"
        lib.probe_motor()
        check_state(f"follower motor tick {tick}", cpu, lib)
        assert writes == ordered_writes(lib)
        if tick == 32:
            assert ram[0x105] == 18 and ram[0x2C] == 58
        if tick % 2 == 0:
            cpu.mem[0x84] = ram[0x84] = 12 if tick < 32 else 0
            writes.clear()
            cpu.call(0xE87A, max_steps=2000)
            assert cpu.pc == 0xFFFE
            lib.probe_follower()
            check_state(f"follower request tick {tick}", cpu, lib)
            assert writes == ordered_writes(lib)
    assert ram[0x105] < 18 and ram[0x2C] < 58
    print("IAC throttle opening and release: 97 motor and 49 request services matched")


def serial(make_cpu, rom, lib):
    for case, data in enumerate(([0], [1], [2, 0, 0x20], [3, 0, 0x20, 0, 0x40],
                 [4, 0, 0, 0x30, 0, 0, 0, 0, 0, 0, 0, 0, 0x20], [0])):
        cpu = make_cpu()
        hardware(cpu, rom, lib)
        frame = [128, 0x55 + len(data), *data]
        frame.append(-sum(frame) & 255)
        if case == 5:
            frame[-1] ^= 1
        cpu.sp = 0x1F6
        tx = []
        cpu.on_write(0x400A, 0x400A, lambda addr, value: tx.append(value))
        cpu.mem[0x4007] = 0x27
        for byte in frame:
            cpu.mem[0x4009] = byte
            assert cpu.mem[0x4007] & 0x20
            cpu.mem[0x4008] = 0x20
            cpu.sp = 0x1F6
            cpu.write16(0x1FE, 0xCCC6)
            cpu.pc = cpu.read16(0xFFF2)
            assert cpu.run(until_pc=0xCCC6, max_steps=3000) == "until_pc"
        status = lib.probe_frame(bytes(frame), len(frame))
        if case == 5:
            assert status == 3 and not tx and (cpu.mem[0x12D] & 15) == 3
            print("SCI bad checksum: both receivers rejected the frame")
            continue
        assert status == 0
        for _ in range(82):
            if cpu.mem[0x4007] == 0x41:
                break
            cpu.mem[0x4008] = 0x80
            cpu.sp = 0x1F6
            cpu.write16(0x1FE, 0xCCC6)
            cpu.pc = cpu.read16(0xFFF2)
            assert cpu.run(until_pc=0xCCC6, max_steps=3000) == "until_pc"
        assert cpu.mem[0x4007] == 0x41
        expected = [lib.probe_response(i) for i in range(lib.probe_response_length())]
        assert tx == expected, (data[0], tx, expected)
        print(f"SCI Mode {data[0]}: {len(tx)} transmitted bytes matched")


# Each row exercises the last counting pass and the first qualifying pass.
# The last tuple is (latch address, mask). A fresh fixture isolates faults
# which would otherwise inhibit one another in the same qualification chain.
FAULT_TRANSITIONS = [
    (13, {0x1B: 120, 0x6F: 100, 0x3F: 16, 0x5B: 147, 0x82: 14, 0xAA: 30}, (0x4C, 0x40)),
    (21, {0x81: 129, 0xB6: 11, 0xA7: 20}, (0x4C, 0x04)),
    (24, {0x3F: 8, 0x56: 41, 0x37: 0, 0x82: 0, 0x63: 0, 0xAB: 2}, (0x4D, 0x80)),
    (32, {0x2E: 32, 0x5B: 173, 0x112: 134, 0x82: 16, 0xAE: 10}, (0x4D, 0x08)),
    (33, {0x7F: 101, 0xB6: 46, 0xB0: 8}, (0x4D, 0x04)),
    (34, {0x56: 24, 0x82: 16, 0xB5: 1}, (0x4D, 0x02)),
    (42, {0x34: 64, 0x56: 5, 0x36: 0}, (0x01, 0x80)),
    (44, {0x3F: 8, 0x44: 128, 0x73: 44, 0xA8: 20}, (0x4E, 0x08)),
    (45, {0x3F: 8, 0x44: 128, 0x73: 159, 0x82: 6, 0xA9: 50}, (0x4E, 0x04)),
]


def diagnostic_transitions(make_cpu, rom, lib):
    def fixture(inputs):
        cpu = make_cpu()
        ram, _, _ = hardware(cpu, rom, lib)
        for addr, value in inputs.items():
            cpu.mem[addr] = ram[addr] = value
        cpu.timer_writes = []

        def write(addr, value):
            cpu.mem[addr] = value
            cpu.timer_writes.append((addr, value))

        cpu.on_write(0x3FC0, 0x3FFF, write)
        return cpu, ram

    def service(cpu, label, segment=False):
        cpu.timer_writes.clear()
        cpu.sp = 0x1FF
        entry = cpu.read16(0xCCE4 + 2 * 13) if segment else 0xE4F7
        cpu.call(entry, max_steps=3000)
        assert cpu.pc == 0xFFFE, label
        if segment:
            lib.probe_major(13)
        else:
            lib.probe_qualification()
        check_state(label, cpu, lib)
        assert cpu.timer_writes == ordered_writes(lib), (label, cpu.timer_writes, ordered_writes(lib))

    for code, inputs, (address, mask) in FAULT_TRANSITIONS:
        cpu, ram = fixture(inputs)
        assert not ram[address] & mask
        for count in (1, 2):
            label = f"Error {code}, qualification pass {count}"
            service(cpu, label)
            assert bool(ram[address] & mask) == (count == 2), label
        print(f"Error {code}: latched on the second qualifying service")

    # Recovery, one-shot inputs, and the two lower saturation boundaries.
    for name, inputs, expected in [
        ("O2 low-load decay", {0x41:64, 0x6F:100, 0x3F:16, 0x5B:147, 0xAA:2}, {0xAA:1}),
        ("EGR timer decay", {0x2E:32, 0x5B:173, 0xAF:3}, {0xAF:1}),
        ("EGR timer floor", {0x2E:32, 0x5B:173, 0xAF:1}, {0xAE:0, 0xAF:0}),
        ("Error 41 pulse", {0x40:1}, {0x40:0, 0x4E:0x40}),
        ("EST recovery", {0x34:64, 0x56:5, 0x3F:1, 0xB4:1}, {0x3F:0, 0xB4:0}),
        ("Error 43 knock input", {0x3B:32}, {0x4E:16}),
        ("rich O2 at low TPS", {0x44:128, 0x73:159, 0x82:5, 0xA9:50}, {0xA9:0}),
    ]:
        cpu, ram = fixture(inputs)
        service(cpu, name)
        for addr, value in expected.items():
            assert ram[addr] == value, (name, hex(addr), ram[addr], value)
        print(f"{name}: RAM and MPU matched")

    # Keep the two machines independent through qualification, confirmation,
    # retained-code checksum and the service-engine lamp transition.
    cpu, ram = fixture({0x34:128, 0x81:129, 0xA7:20, 0xB6:11})
    for count in range(1, 53):
        label = f"Segment D high TPS, pass {count}"
        service(cpu, label, segment=True)
        assert bool(ram[5] & 4) == (count == 52), label
        assert bool(cpu.mem[0x3FFD] & 8) == (count < 52), label
    assert ram[0x18] * 256 + ram[0x19] == 1 + sum(ram[5:10])
    print("Segment D: retained Error 21, checksum and lamp matched over 52 services")
