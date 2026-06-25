#!/usr/bin/env python3
"""Create a release-ready .arduboy package from the compiled HEX and banner."""

from __future__ import annotations

import argparse
import json
import shutil
import tempfile
import zipfile
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--hex", required=True, type=Path)
    parser.add_argument("--banner", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--version", default="dev")
    return parser.parse_args()


def package_info(hex_name: str, banner_name: str, version: str) -> dict:
    return {
        "schemaVersion": 2,
        "title": "All Men's Morris",
        "description": "Morris-family tabletop strategy for Arduboy and Arduboy FX.",
        "author": "Jujuyeh",
        "version": version,
        "genre": "Tabletop",
        "publisher": "Jujuyeh",
        "code": "Jujuyeh with AI-assisted development",
        "art": "Jujuyeh",
        "sound": "Jujuyeh",
        "sourceUrl": "https://github.com/Jujuyeh/all-mens-morris",
        "banner": banner_name,
        "binaries": [
            {
                "title": "All Men's Morris",
                "filename": hex_name,
                "device": "Arduboy",
            }
        ],
        "buttons": [
            {"control": "D-Pad", "action": "Move cursor and navigate menus."},
            {"control": "A", "action": "Quick menu or audio mode in main menu."},
            {"control": "B", "action": "Confirm, place, move, or capture."},
        ],
    }


def main() -> None:
    args = parse_args()
    if not args.hex.is_file():
        raise SystemExit(f"Missing HEX: {args.hex}")
    if not args.banner.is_file():
        raise SystemExit(f"Missing banner: {args.banner}")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    hex_name = "all-mens-morris.hex"
    banner_name = "banner.png"

    with tempfile.TemporaryDirectory() as tmp:
        package_dir = Path(tmp)
        shutil.copyfile(args.hex, package_dir / hex_name)
        shutil.copyfile(args.banner, package_dir / banner_name)
        (package_dir / "info.json").write_text(
            json.dumps(package_info(hex_name, banner_name, args.version), indent=2) + "\n",
            encoding="utf-8",
        )

        with zipfile.ZipFile(args.output, "w", compression=zipfile.ZIP_DEFLATED) as archive:
            for path in sorted(package_dir.iterdir()):
                archive.write(path, path.name)


if __name__ == "__main__":
    main()
