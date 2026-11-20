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
    0xE87A: "IAC motor request",
    0xF67B: "injector service", 0xF76A: "airflow service",
    0xFA91: "SCI receive", 0xFBBB: "SCI transmit", 0xFCA2: "factory test",
    0xFDED: "factory PWM and fuel/spark exercise",
}


# Split at direct transfer targets and after transfers (including calls).
TRANSFERS = set("""JMP JSR BSR RTS RTI SWI WAI STOP TEST BRA BRN
    BHI BLS BCC BCS BNE BEQ BVC BVS BPL BMI BGE BLT BGT BLE BHS BLO
    BRSET BRCLR""".split())


def check_unrun_audit(audit, blocks):
    expected = {(b[0], b[-1]): len(b) for b in blocks}
    documented = {}
    for line in audit.read_text().splitlines():
        if not line.startswith("| `$"):
            continue
        row = re.fullmatch(r"\| `\$([0-9A-F]{4})(?:–\$([0-9A-F]{4}))?` \| (\d+) \| (.+) \|", line)
        if not row:
            raise ValueError(f"Malformed block row: {line}")
        first, last, count, _ = row.groups()
        key = (int(first, 16), int(last or first, 16))
        if key in documented:
            raise ValueError(f"Duplicate audit entry at ${first}")
        documented[key] = int(count)
    if documented != expected:
        for first, last in sorted(documented.keys() | expected.keys()):
            if documented.get((first, last)) != expected.get((first, last)):
                print(f"${first:04X}–${last:04X}: listed={documented.get((first,last), 0)} "
                      f"unexecuted={expected.get((first,last), 0)}")
        raise ValueError("The unexecuted ROM inventory differs from this replay run")
    print(f"Audit matched {len(blocks)} blocks and {sum(expected.values())} unexecuted instructions")


def report(listing, rom, visited, audit=None):
    instructions, starts = set(), set(BLOCKS)
    lengths, heads = {}, set()
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
        if audit:
            lengths[address] = len(data)
            if mnemonic.upper() in TRANSFERS:
                heads.add(address + len(data))
                dest = re.search(r"[L$]([0-9a-fA-F]{4})$", operand.strip())
                if dest:
                    heads.add(int(dest[1], 16))
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
    if audit:
        heads.update(starts)
        blocks = []
        for address in sorted(instructions - visited):
            if (not blocks or address in heads or
                    address != blocks[-1][-1] + lengths[blocks[-1][-1]]):
                blocks.append([])
            blocks[-1].append(address)
        check_unrun_audit(audit, blocks)
