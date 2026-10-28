"""Run the linked image in generic C52 uCsim and inspect real XRAM state."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build", required=True, type=Path)
    parser.add_argument("--simulator", default="s51")
    args = parser.parse_args()
    simulator = shutil.which(args.simulator)
    if not simulator:
        parser.error("s51 simulator not found")
    build = args.build.resolve()
    report = json.loads((build / "build-report.json").read_text(encoding="utf-8"))
    image = build / "bluebridge_iap15_policy.ihx"
    if not image.is_file():
        parser.error("Linked IHX missing")
    address = report["diagnostics_xdata_address"]
    commands = (f'file "{image.name}"\nstep 500000\nstate\n'
                f'dx 0x{address:04x} 0x{address + 8:04x}\nquit\n')
    result = subprocess.run([simulator, "-q", "-c", "-", "-t", "C52"],
                            cwd=build, input=commands, text=True, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, timeout=30, check=False)
    (build / "simulator.log").write_text(result.stdout, encoding="utf-8")
    if result.returncode or "no loadable file found" in result.stdout:
        raise RuntimeError("uCsim failed to load or run the linked IHX")
    instruction_match = re.search(r"\bInst=\s*(\d+)", result.stdout)
    stack_match = re.search(r"(?:Most|Max) value of stack pointer=\s*0x([0-9a-f]+)",
                            result.stdout, re.I)
    xram: dict[int, int] = {}
    for line in result.stdout.splitlines():
        match = re.match(r"^0x([0-9a-f]{4})((?:\s+[0-9a-f]{2})+)(?:\s|$)", line, re.I)
        if match:
            base = int(match[1], 16)
            for offset, value in enumerate(match[2].split()):
                xram[base + offset] = int(value, 16)
    if not instruction_match or not stack_match or any(address + i not in xram for i in range(9)):
        raise ValueError("uCsim instruction, stack or XRAM evidence incomplete")
    values = [xram[address + i] for i in range(9)]
    if (int(instruction_match[1]) != 500000 or values[:6] != [4, 2, 2, 7, 1, 3] or
            int(stack_match[1], 16) >= 256):
        raise ValueError("Target diagnostic state does not match synthetic input contract")
    summary = {
        "simulator": Path(simulator).name,
        "cpu_model": "generic C52; no IAP15 peripherals or CT107D wiring modeled",
        "instructions": int(instruction_match[1]),
        "processed": values[0], "accepted": values[1], "rejected": values[2],
        "last_valid_key": values[3], "resource_conflict": values[4],
        "error_latch": values[5], "port_readback": values[6:],
        "stack_pointer_high_water": int(stack_match[1], 16),
    }
    (build / "simulator-report.json").write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))


if __name__ == "__main__":
    main()
