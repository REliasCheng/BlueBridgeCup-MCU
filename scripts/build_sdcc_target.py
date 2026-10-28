"""Build and verify the software-only IAP15F2K61S2 MCS-51 foundation.

All generated files must live in a fresh directory outside this checkout.
The default profile avoids the addresses reserved by STC on-chip emulation.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCES = (
    Path("practice/peripheral-driver-corrections/peripheral_policy.c"),
    Path("target/sdcc/ct107d_platform.c"),
    Path("target/sdcc/main.c"),
)
IMAGE = "bluebridge_iap15_policy"
PROFILES = {
    "on-chip-emulation": {"code": 55 * 1024, "xram": 1024},
    "standalone": {"code": 61 * 1024, "xram": 1792},
}
MIN_STACK = 64


def run(command: list[str], log: list[str]) -> str:
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, check=False, timeout=60)
    log.extend(("COMMAND: " + " ".join(command), result.stdout,
                f"EXIT: {result.returncode}"))
    if result.returncode:
        raise RuntimeError(f"Command failed ({result.returncode}): {command[0]}")
    return result.stdout


def read_hex(path: Path, code_limit: int) -> dict[int, int]:
    memory: dict[int, int] = {}
    eof = False
    for number, line in enumerate(path.read_text(encoding="ascii").splitlines(), 1):
        if eof or not line.startswith(":"):
            raise ValueError(f"Invalid Intel HEX record at line {number}")
        try:
            record = bytes.fromhex(line[1:])
        except ValueError as exc:
            raise ValueError(f"Non-hex Intel HEX record at line {number}") from exc
        if len(record) < 5 or len(record) != record[0] + 5 or sum(record) % 256:
            raise ValueError(f"Invalid Intel HEX length/checksum at line {number}")
        address = (record[1] << 8) | record[2]
        kind = record[3]
        if kind == 0:
            for offset, value in enumerate(record[4:-1]):
                position = address + offset
                if position >= code_limit or position in memory:
                    raise ValueError(f"Overlapping/out-of-range code at {position:#x}")
                memory[position] = value
        elif kind == 1 and record[0] == 0 and address == 0:
            eof = True
        else:
            raise ValueError(f"Unexpected Intel HEX record type {kind} at line {number}")
    if not eof or not memory or 0 not in memory:
        raise ValueError("Intel HEX lacks data, reset vector or EOF")
    return memory


def symbol(map_text: str, name: str, space: str) -> int:
    found = re.search(rf"(?m)^{space}:\s+([0-9A-Fa-f]{{8}})\s+_{re.escape(name)}\b",
                      map_text)
    if not found:
        raise ValueError(f"Linked {space} symbol absent: {name}")
    return int(found[1], 16)


def memory_report(path: Path, profile: dict[str, int]) -> dict[str, int]:
    content = path.read_text(encoding="ascii")
    stack = re.search(r"Stack starts at: 0x([0-9a-f]+).*?with (\d+) bytes available", content)
    xdata = re.search(r"^\s*EXTERNAL RAM\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\d+)\s+(\d+)", content, re.M)
    code = re.search(r"^\s*ROM/EPROM/FLASH\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\d+)\s+(\d+)", content, re.M)
    if not stack or not xdata or not code:
        raise ValueError("Incomplete SDCC .mem report")
    layout: dict[int, str] = {}
    for line in content.splitlines():
        match = re.match(r"^0x([0-9a-f]{2}):\|([^|](?:\|[^|]){15})\|$", line)
        if match:
            start = int(match[1], 16)
            for offset, cell in enumerate(match[2].split("|")):
                layout[start + offset] = cell
    if len(layout) != 256:
        raise ValueError("Incomplete 256-byte IRAM layout")
    values = {
        "code_bytes": int(code[3]), "code_end": int(code[2], 16),
        "code_limit_bytes": int(code[4]),
        "direct_data_bytes": sum(1 for addr, cell in layout.items() if addr < 128 and cell.islower()),
        "idata_allocated_bytes": sum(1 for cell in layout.values() if cell == "I"),
        "stack_reserved_bytes": sum(1 for cell in layout.values() if cell == "S"),
        "stack_start": int(stack[1], 16),
        "stack_reported_available_bytes": int(stack[2]),
        "xdata_start": int(xdata[1], 16), "xdata_end": int(xdata[2], 16),
        "xdata_bytes": int(xdata[3]), "xdata_limit_bytes": int(xdata[4]),
    }
    if (values["code_limit_bytes"] != profile["code"] or
            values["code_end"] >= profile["code"] or
            values["xdata_limit_bytes"] != profile["xram"] or
            values["xdata_end"] >= profile["xram"] or
            values["stack_reported_available_bytes"] < MIN_STACK or
            values["stack_start"] + MIN_STACK > 256):
        raise ValueError("SDCC memory allocation violates selected IAP15 profile")
    return values


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sdcc", default="sdcc")
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--profile", choices=PROFILES, default="on-chip-emulation")
    args = parser.parse_args()
    executable = shutil.which(args.sdcc)
    if not executable:
        parser.error("SDCC executable not found")
    sdcc = Path(executable).resolve()
    packihx = sdcc.with_name("packihx.exe" if sdcc.suffix.lower() == ".exe" else "packihx")
    if not packihx.is_file():
        found = shutil.which("packihx")
        if not found:
            parser.error("packihx executable not found")
        packihx = Path(found).resolve()
    output = args.output.resolve()
    if output == ROOT or ROOT in output.parents or output.exists():
        parser.error("Use a new output directory outside the repository")
    output.mkdir(parents=True)
    profile = PROFILES[args.profile]
    log: list[str] = []
    try:
        version = run([str(sdcc), "-v"], log).strip()
        if "mcs51" not in version.lower():
            raise ValueError("SDCC lacks MCS-51 backend")
        objects = []
        for source in SOURCES:
            obj = output / (source.stem + ".rel")
            run([str(sdcc), "-mmcs51", "--std-c11", "--model-small", "--stack-auto",
                 "--Werror", "-Ipractice/peripheral-driver-corrections", "-Itarget/sdcc",
                 "-c", str(source), "-o", str(obj)], log)
            objects.append(str(obj))
        ihx = output / (IMAGE + ".ihx")
        run([str(sdcc), "-mmcs51", "--model-small", "--stack-auto",
             "--iram-size", "256", "--xram-loc", "0", "--xram-size", str(profile["xram"]),
             "--code-size", str(profile["code"]), "--stack-size", str(MIN_STACK),
             "--out-fmt-ihx", "-o", str(ihx), *objects], log)
        packed = subprocess.run([str(packihx), str(ihx)], cwd=ROOT, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, check=False, timeout=30)
        log.extend(("COMMAND: " + str(packihx) + " " + str(ihx),
                    packed.stderr.decode("utf-8", errors="replace"),
                    f"EXIT: {packed.returncode}"))
        if packed.returncode:
            raise RuntimeError("packihx failed")
        hex_path = output / (IMAGE + ".hex")
        hex_path.write_bytes(packed.stdout)
        ihx_memory = read_hex(ihx, profile["code"])
        hex_memory = read_hex(hex_path, profile["code"])
        if ihx_memory != hex_memory:
            raise ValueError("Packed HEX differs from linked IHX")
        map_text = (output / (IMAGE + ".map")).read_text(encoding="ascii")
        symbols = {name: symbol(map_text, name, "C") for name in
                   ("main", "PeripheralPolicy_KeyValue", "Ct107d_ProcessSyntheticKey",
                    "Ct107d_CheckPinConflict", "Iap15_CaptureReadOnlyPorts",
                    "ct107d_abi_sizes")}
        diagnostics_address = symbol(map_text, "ct107d_diagnostics", "D")
        assembly = (output / "ct107d_platform.asm").read_text(encoding="ascii")
        if "lcall\t_PeripheralPolicy_KeyValue" not in assembly and "lcall _PeripheralPolicy_KeyValue" not in assembly:
            raise ValueError("Original policy call absent from target assembly")
        abi = [hex_memory[symbols["ct107d_abi_sizes"] + offset] for offset in range(3)]
        if abi[0] != 9 or abi[1] == 0 or abi[2] == 0:
            raise ValueError("Unexpected target diagnostic object or pointer ABI")
        memory = memory_report(output / (IMAGE + ".mem"), profile)
        if diagnostics_address + abi[0] > profile["xram"]:
            raise ValueError("Diagnostics overlap reserved XRAM")
        if re.search(r"(?im)^.*\b(?:warning|error)\s*[:0-9]", "\n".join(log)):
            raise ValueError("Compiler or linker diagnostic found")
        report = {
            "target": "IAP15F2K61S2 software-only MCS-51 peripheral policy foundation",
            "profile": args.profile, "compiler": version,
            "sources": [str(s) for s in SOURCES],
            "startup_runtime": "SDCC-provided generic MCS-51 startup/runtime; no custom board startup",
            "memory_model": "small + stack-auto",
            "memory": memory, "abi_sizes": dict(zip(("diagnostics_struct", "generic_void_pointer", "function_pointer"), abi)),
            "linked_symbols": symbols, "diagnostics_xdata_address": diagnostics_address,
            "hex_size_bytes": hex_path.stat().st_size,
            "hex_sha256": hashlib.sha256(hex_path.read_bytes()).hexdigest(),
            "hex_data_bytes": len(hex_memory), "warnings": 0, "errors": 0,
            "board_runtime_verified": False,
        }
        (output / "build-report.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps(report, indent=2))
    finally:
        (output / "build.log").write_text("\n".join(log) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
