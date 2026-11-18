"""Classify source definitions by static call paths from the live IRQ (GCC)."""
import gzip
import json
import re
import subprocess
import tempfile
from pathlib import Path

root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as tmp:
    graph, declarations = Path(tmp)/"calls", Path(tmp)/"definitions"
    subprocess.run(["gcc", "-std=c89", "-O0", f"-fdump-ipa-cgraph={graph}",
                    "-aux-info", str(declarations), "-c", "main.c",
                    "-o", str(Path(tmp)/"main.o")], cwd=root, check=True)
    calls = {}
    for name, callees in re.findall(r"^(\w+)/\d+ \([^\n]+\)\n(?:(?!^\S)[^\n]*\n)*?  Calls:([^\n]*)",
                                    graph.read_text(), re.M):
        calls.setdefault(name, set()).update(re.findall(r"(\w+)/\d+", callees))
    definitions = dict((name, file) for file, name in re.findall(
        r"^/\* (src/[^:]+):\d+:\wF \*/ .*?\b(\w+) \(", declarations.read_text(), re.M))
    # Collect dynamic evidence without calling a test suite or any spark helper.
    driver = Path(tmp)/"drive.c"
    driver.write_text('#define main historical_harness\n#include "main.c"\n#undef main\n'
                      'void __gcov_reset(void);\n'
                      'int main(void) { unsigned i; ecm_reset(); __gcov_reset(); '
                      'for (i=0;i<1024;++i) { '
                      'sim_set_ecm_reference_rpm(i<64 ? 0 : (i<512 ? 800 : 2400)); '
                      'irq_6p25ms(); } return 0; }\n')
    subprocess.run(["gcc", "-std=c89", "--coverage", "-I", str(root), "drive.c",
                    "-o", "drive"], cwd=tmp, check=True)
    subprocess.run(["./drive"], cwd=tmp, check=True)
    subprocess.run(["gcov", "--json-format", "drive.gcda"], cwd=tmp,
                   stdout=subprocess.DEVNULL, check=True)
    with gzip.open(next(Path(tmp).glob("*.gcov.json.gz")), "rt") as data:
        observed = {f["name"] for unit in json.load(data)["files"]
                    for f in unit["functions"] if f["execution_count"]}
seen, pending = set(), ["irq_6p25ms"]
while pending:
    name = pending.pop()
    if name not in seen:
        seen.add(name)
        pending.extend(calls.get(name, ()))
assert "bua_spark_timing_output_ld1f6" in seen
for name, file in sorted(definitions.items(), key=lambda item: (item[1], item[0])):
    status = "observed IRQ" if name in seen and name in observed else (
        "static path only" if name in seen else "outside IRQ")
    print(f"{status}\t{file}\t{name}")
