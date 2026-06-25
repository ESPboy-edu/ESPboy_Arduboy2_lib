#!/usr/bin/env python3
"""Local TableTop Studio server for Morris board profiles."""

from __future__ import annotations

import argparse
import base64
import json
import re
import sys
import webbrowser
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
BOARDS_DIR = ROOT / "boards"
sys.path.insert(0, str(ROOT / "tools" / "music"))
import audio_data  # noqa: E402

GLOBAL_ASSET_PATHS = {
    "assets/fx/banner.png",
    "assets/ui/pieces.png",
}


def safe_slug(value: str) -> str:
    cleaned = re.sub(r"[^a-z0-9_-]+", "-", value.lower()).strip("-")
    return cleaned or "board"


class TableTopStudioHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(ROOT), **kwargs)

    def log_message(self, format: str, *args) -> None:
        print(f"tabletop-studio: {format % args}")

    def do_GET(self) -> None:
        if self.path == "/api/boards":
            boards = []
            for path in sorted(BOARDS_DIR.glob("*.json")):
                try:
                    data = json.loads(path.read_text(encoding="utf-8"))
                except json.JSONDecodeError as exc:
                    boards.append(
                        {
                            "slug": path.stem,
                            "path": str(path.relative_to(ROOT)),
                            "error": str(exc),
                        }
                    )
                    continue
                boards.append(
                    {
                        "slug": path.stem,
                        "path": str(path.relative_to(ROOT)),
                        "id": data.get("id", path.stem),
                        "name": data.get("name", path.stem),
                        "label": data.get("label", path.stem.upper()),
                        "data": data,
                    }
                )
            self.send_json({"count": len(boards), "boards": boards})
            return
        if self.path == "/api/audio":
            menu_music = audio_data.load_menu_music(ROOT)
            effects = audio_data.load_effects(ROOT)
            self.send_json(
                {
                    "count": 1 + len(effects),
                    "sounds": [menu_music] + effects,
                }
            )
            return
        super().do_GET()

    def do_POST(self) -> None:
        if self.path == "/api/save-board":
            self.save_board()
            return
        if self.path == "/api/save-global-asset":
            self.save_global_asset()
            return
        if self.path == "/api/save-audio":
            self.save_audio()
            return
        self.send_error(404)

    def read_payload(self) -> dict:
        length = int(self.headers.get("Content-Length", "0"))
        return json.loads(self.rfile.read(length).decode("utf-8"))

    def save_board(self) -> None:
        try:
            payload = self.read_payload()
            data = payload.get("data")
            if not isinstance(data, dict):
                raise ValueError("Board payload must include a data object")
            board_id = str(data.get("id") or data.get("name") or "board")
            slug = safe_slug(str(payload.get("slug") or board_id))
            data.setdefault("schemaVersion", 1)
            data["id"] = safe_slug(board_id)
            data.setdefault("name", board_id)
            data.setdefault("label", data["id"].upper())

            BOARDS_DIR.mkdir(parents=True, exist_ok=True)
            path = BOARDS_DIR / f"{slug}.json"
            path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
            self.send_json(
                {
                    "ok": True,
                    "slug": slug,
                    "path": str(path.relative_to(ROOT)),
                    "data": data,
                }
            )
        except Exception as exc:
            self.send_json({"ok": False, "error": str(exc)}, status=400)

    def save_global_asset(self) -> None:
        try:
            payload = self.read_payload()
            asset_path = str(payload.get("path") or "")
            data_url = str(payload.get("dataUrl") or "")
            if asset_path not in GLOBAL_ASSET_PATHS:
                raise ValueError("Unsupported global asset path")
            prefix = "data:image/png;base64,"
            if not data_url.startswith(prefix):
                raise ValueError("Asset payload must be a PNG data URL")

            path = ROOT / asset_path
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(base64.b64decode(data_url[len(prefix):], validate=True))
            self.send_json({"ok": True, "path": asset_path})
        except Exception as exc:
            self.send_json({"ok": False, "error": str(exc)}, status=400)

    def save_audio(self) -> None:
        try:
            payload = self.read_payload()
            kind = str(payload.get("kind") or "")
            if kind == "music" or payload.get("id") == "menu-music":
                saved = audio_data.save_menu_music(ROOT, payload)
            elif kind == "effect":
                saved = audio_data.save_effect(ROOT, payload)
            else:
                raise ValueError("Audio payload kind must be music or effect")
            self.send_json({"ok": True, "sound": saved})
        except Exception as exc:
            self.send_json({"ok": False, "error": str(exc)}, status=400)

    def send_json(self, payload: dict, status: int = 200) -> None:
        body = json.dumps(payload, indent=2).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8124)
    parser.add_argument("--open", action="store_true")
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    url = f"http://{args.host}:{args.port}/tools/tabletop-studio/"
    server = ThreadingHTTPServer((args.host, args.port), TableTopStudioHandler)
    print(f"TableTop Studio: {url}")
    print("Stop with Ctrl-C.")
    if args.open:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print()
    finally:
        server.server_close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
