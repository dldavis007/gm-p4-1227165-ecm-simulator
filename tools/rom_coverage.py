"""Static instruction starts and reporting regions from the assembled BUA listing."""
import bisect
from collections import defaultdict
import re


# Include scheduler dispatches, vectors and branch-entered functional blocks
# alongside ordinary JSR/BSR targets. Regions are contiguous address ranges.
BLOCKS = {
    0xD9D7: "power enrichment qualification", 0xD86B: "fuel transients",
    0xC800: "reset", 0xCB5F: "Mode 4 dispatch", 0xD41F: "idle setpoint",
    0xD59A: "idle PID", 0xD6D1: "odd fuel / shutdown", 0xDA28: "enrichment",
    0xDEE5: "EGR", 0xE10F: "RPM/VSS ratio", 0xE4F7: "fault qualification",
    0xE75D: "purge", 0xEDA3: "segment 1 outputs", 0xEF04: "diagnostics",
    0xEFCC: "field service", 0xF10E: "code flash", 0xF4DF: "IAC service",
    0xF67B: "injector service", 0xF76A: "airflow service",
    0xFA91: "SCI receive", 0xFBBB: "SCI transmit", 0xFCA2: "factory test",
}


def report(listing, rom, visited):
    instructions, starts = set(), set(BLOCKS)
    for line in listing.read_text(encoding="latin1").splitlines():
        match = re.match(r"^\s*\d+ ([0-9a-f]{4}) ((?:[0-9a-f]{2} )+)\s*(?:\w+:\s*)?(\w+)\s*([^;]*)", line)
        if not match:
            continue
        address, encoded, mnemonic, operand = match.groups()
        address = int(address, 16)
        if mnemonic.upper() in {"FCB", "FDB", "FCC", "RMB", "ORG", "EQU"}:
            continue
        data = bytes.fromhex(encoded)
        offset = address - 0xC000
        if rom[offset:offset+len(data)] != data:
            raise ValueError(f"Listing differs from ROM at ${address:04X}")
        instructions.add(address)
        target = re.fullmatch(r"[L$]([0-9a-fA-F]{4})", operand.strip())
        if mnemonic.upper() in {"JSR", "BSR"} and target:
            starts.add(int(target[1], 16))
    for offset in [*range(0xCE4, 0xD04, 2), *range(0x3FF0, 0x4000, 2)]:
        starts.add(int.from_bytes(rom[offset:offset+2], "big"))
    starts = sorted((starts & instructions) | {min(instructions)})
    groups = defaultdict(set)
    for addr in instructions:
        groups[starts[bisect.bisect_right(starts, addr)-1]].add(addr)
    for start in starts:
        group = groups[start]
        count = len(group & visited)
        print(f"${start:04X} {count:4}/{len(group):4} {100*count/len(group):6.1f}%"
              f"  {len(group)-count:4} unrun  {BLOCKS.get(start, '')}")
    count = len(instructions & visited)
    print(f"Listing instructions executed: {count}/{len(instructions)} ({100*count/len(instructions):.2f}%)")
    other = sorted(visited - instructions)
    if other:
        print("Executed PCs outside listing instruction starts:", " ".join(f"${x:04X}" for x in other))
