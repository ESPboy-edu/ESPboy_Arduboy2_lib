#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2026 MostlyBuilds

"""Build the Mostly Logic HEX and deterministic Arduboy package."""

from __future__ import annotations

import argparse
import datetime
import json
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
CONFIG_PATH = ROOT / "packaging" / "arduboy-package.json"
FIRMWARE_PATH = ROOT / ".pio" / "build" / "arduboy" / "firmware.hex"
OUTPUT_NAMES = ("MostlyLogic.hex", "MostlyLogic.arduboy")
ZIP_TIMESTAMP = (1980, 1, 1, 0, 0, 0)
SEMANTIC_VERSION = re.compile(r"^[0-9]+\.[0-9]+\.[0-9]+$")


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check",
        action="store_true",
        help="verify that the committed artifacts match a fresh build",
    )
    return parser.parse_args()


def find_platformio() -> str:
    executable = shutil.which("pio")
    if executable:
        return executable

    fallback = Path.home() / ".platformio" / "penv" / "bin" / "pio"
    if fallback.is_file():
        return str(fallback)

    raise SystemExit("PlatformIO was not found; install it or add 'pio' to PATH.")


def build_firmware() -> None:
    subprocess.run([find_platformio(), "run", "-e", "arduboy"], cwd=ROOT, check=True)
    if not FIRMWARE_PATH.is_file():
        raise SystemExit(f"PlatformIO did not produce {FIRMWARE_PATH}")


def read_metadata() -> dict:
    try:
        metadata = json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise SystemExit(f"Could not read package metadata: {error}") from error
    if not isinstance(metadata, dict):
        raise SystemExit("Package metadata must be a JSON object.")
    required = {
        "schemaVersion",
        "title",
        "description",
        "author",
        "version",
        "date",
        "genre",
        "licenseFile",
        "url",
        "sourceUrl",
        "email",
        "companion",
        "contributors",
        "binaries",
    }
    missing = required - metadata.keys()
    if missing:
        raise SystemExit(f"Package metadata is missing: {', '.join(sorted(missing))}")
    if metadata["schemaVersion"] != 4:
        raise SystemExit("Package metadata must use Arduboy schema version 4.")
    required_text = (
        "title",
        "description",
        "author",
        "version",
        "date",
        "genre",
        "licenseFile",
        "url",
        "sourceUrl",
        "email",
    )
    for field in required_text:
        if not isinstance(metadata[field], str) or not metadata[field].strip():
            raise SystemExit(f"Package metadata field {field!r} must be a non-empty string.")
    if not isinstance(metadata["companion"], str):
        raise SystemExit("Package metadata field 'companion' must be a string.")
    if not SEMANTIC_VERSION.fullmatch(metadata["version"]):
        raise SystemExit("Package version must use MAJOR.MINOR.PATCH format.")
    try:
        datetime.date.fromisoformat(metadata["date"])
    except ValueError as error:
        raise SystemExit("Package date must use YYYY-MM-DD format.") from error
    for field in ("url", "sourceUrl"):
        if not metadata[field].startswith(("https://", "http://")):
            raise SystemExit(f"Package metadata field {field!r} must be an HTTP(S) URL.")
    if "@" not in metadata["email"]:
        raise SystemExit("Package email must contain '@'.")
    if not isinstance(metadata["contributors"], list):
        raise SystemExit("Package contributors must be an array.")
    for index, contributor in enumerate(metadata["contributors"]):
        if not isinstance(contributor, dict):
            raise SystemExit(f"Contributor {index} must be an object.")
        for field in ("name", "roles"):
            if field not in contributor:
                raise SystemExit(f"Contributor {index} is missing {field!r}.")
        if not isinstance(contributor["name"], str) or not contributor["name"].strip():
            raise SystemExit(f"Contributor {index} must have a non-empty name.")
        if not isinstance(contributor["roles"], list) or not contributor["roles"] or not all(
            isinstance(role, str) and role.strip() for role in contributor["roles"]
        ):
            raise SystemExit(f"Contributor {index} roles must be non-empty strings.")
    if not isinstance(metadata["binaries"], list):
        raise SystemExit("Package binaries must be an array.")
    if len(metadata["binaries"]) != 1:
        raise SystemExit("Mostly Logic must have exactly one packaged binary.")

    binary = metadata["binaries"][0]
    if not isinstance(binary, dict):
        raise SystemExit("Package binary must be an object.")
    binary_required = {"title", "filename", "device", "cartImage"}
    missing = binary_required - binary.keys()
    if missing:
        raise SystemExit(f"Binary metadata is missing: {', '.join(sorted(missing))}")
    for field in binary_required:
        if not isinstance(binary[field], str) or not binary[field].strip():
            raise SystemExit(f"Binary metadata field {field!r} must be a non-empty string.")
    if binary["device"] not in {"Arduboy", "ArduboyFX", "ArduboyMini"}:
        raise SystemExit(f"Unsupported Arduboy device: {binary['device']}")
    if binary["filename"] != OUTPUT_NAMES[0]:
        raise SystemExit(f"Binary filename must be {OUTPUT_NAMES[0]!r}.")
    if (
        Path(binary["cartImage"]).name != binary["cartImage"]
        or "/" in binary["cartImage"]
        or "\\" in binary["cartImage"]
    ):
        raise SystemExit("Cart image must be a filename without directories.")

    license_path = (ROOT / metadata["licenseFile"]).resolve()
    if license_path.parent != ROOT or not license_path.is_file():
        raise SystemExit("licenseFile must name a file in the repository root.")

    return metadata


def validate_launcher_image(image_path: Path) -> None:
    try:
        data = image_path.read_bytes()
    except OSError as error:
        raise SystemExit(f"Could not read launcher artwork: {error}") from error
    if len(data) < 26 or data[:8] != b"\x89PNG\r\n\x1a\n" or data[12:16] != b"IHDR":
        raise SystemExit(f"Launcher artwork is not a valid PNG: {image_path}")

    width, height, bit_depth, color_type = struct.unpack(">IIBB", data[16:26])
    if (width, height, bit_depth, color_type) != (128, 64, 1, 0):
        raise SystemExit(
            "Launcher artwork must be a 128x64 one-bit grayscale PNG; "
            f"found {width}x{height}, bit depth {bit_depth}, color type {color_type}."
        )


def zip_entry(name: str, data: bytes) -> tuple[zipfile.ZipInfo, bytes]:
    # Fixed timestamps and permissions make repeated package builds byte-identical.
    entry = zipfile.ZipInfo(name, ZIP_TIMESTAMP)
    entry.compress_type = zipfile.ZIP_DEFLATED
    entry.external_attr = 0o100644 << 16
    entry.create_system = 3
    return entry, data


def write_artifacts(output_dir: Path) -> None:
    metadata = read_metadata()
    binary = metadata["binaries"][0]
    image_path = ROOT / "packaging" / binary["cartImage"]
    validate_launcher_image(image_path)
    output_dir.mkdir(parents=True, exist_ok=True)

    hex_data = FIRMWARE_PATH.read_bytes()
    image_data = image_path.read_bytes()
    (output_dir / binary["filename"]).write_bytes(hex_data)

    license_path = ROOT / metadata.pop("licenseFile")
    metadata["license"] = license_path.read_text(encoding="utf-8")
    info_data = (json.dumps(metadata, indent=2, ensure_ascii=False) + "\n").encode("utf-8")

    archive_path = output_dir / "MostlyLogic.arduboy"
    entries = [
        zip_entry("info.json", info_data),
        zip_entry(binary["filename"], hex_data),
        zip_entry(binary["cartImage"], image_data),
    ]
    with zipfile.ZipFile(archive_path, "w") as archive:
        for entry, data in entries:
            archive.writestr(entry, data, compresslevel=9)


def check_artifacts() -> None:
    with tempfile.TemporaryDirectory(prefix="mostly-logic-release-") as temp_dir:
        generated = Path(temp_dir)
        write_artifacts(generated)
        mismatches = [
            name for name in OUTPUT_NAMES if not (ROOT / "dist" / name).is_file()
            or (ROOT / "dist" / name).read_bytes() != (generated / name).read_bytes()
        ]
    if mismatches:
        raise SystemExit(
            "Committed release artifacts are stale: "
            + ", ".join(mismatches)
            + ". Run scripts/build-release.py."
        )
    print("Committed release artifacts match a fresh build.")


def main() -> int:
    arguments = parse_arguments()
    build_firmware()
    if arguments.check:
        check_artifacts()
    else:
        write_artifacts(ROOT / "dist")
        print(f"Wrote {ROOT / 'dist' / OUTPUT_NAMES[0]}")
        print(f"Wrote {ROOT / 'dist' / OUTPUT_NAMES[1]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
