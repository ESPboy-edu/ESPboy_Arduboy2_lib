#!/usr/bin/env python3
"""Generate firmware board/rule data from TableTop Studio profiles."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
MAX_POINT_COUNT = 36
MAX_MILL_COUNT = 24
MAX_ADJACENCY_SLOTS = 8
EMPTY_ADJACENCY = 255


def symbol_prefix(board_id: str) -> str:
    if board_id == "classic-nine":
        return "Classic"
    parts = re.split(r"[^a-zA-Z0-9]+", board_id)
    return "".join(part[:1].upper() + part[1:] for part in parts if part)


def load_profile(path: Path) -> dict:
    data = json.loads(path.read_text(encoding="utf-8"))
    validate_profile(path, data)
    return data


def require_bool(path: Path, rules: dict, key: str) -> bool:
    value = rules.get(key)
    if not isinstance(value, bool):
        raise ValueError(f"{path}: rules.{key} must be boolean")
    return value


def require_uint8(path: Path, rules: dict, key: str) -> int:
    value = rules.get(key)
    if not isinstance(value, int) or not 0 <= value <= 255:
        raise ValueError(f"{path}: rules.{key} must be uint8")
    return value


def require_mill_action(path: Path, rules: dict) -> str:
    value = rules.get("millAction")
    if value not in ("capture", "win"):
        raise ValueError(f"{path}: rules.millAction must be capture or win")
    return value


def validate_profile(path: Path, data: dict) -> None:
    board_id = data.get("id")
    if not isinstance(board_id, str) or not board_id:
        raise ValueError(f"{path}: id is required")
    label = data.get("label")
    if not isinstance(label, str) or not label:
        raise ValueError(f"{path}: label is required")

    canvas = data.get("canvas", {})
    width = canvas.get("width")
    height = canvas.get("height")
    if not isinstance(width, int) or not isinstance(height, int):
        raise ValueError(f"{path}: canvas width/height are required")

    points = data.get("points")
    if not isinstance(points, list) or not points:
        raise ValueError(f"{path}: points must be a non-empty list")
    if len(points) > MAX_POINT_COUNT:
        raise ValueError(f"{path}: point count exceeds {MAX_POINT_COUNT}")
    for index, point in enumerate(points):
        if not isinstance(point, dict):
            raise ValueError(f"{path}: point {index} must be an object")
        x = point.get("x")
        y = point.get("y")
        if not isinstance(x, int) or not 0 <= x < width:
            raise ValueError(f"{path}: point {index} x is outside canvas")
        if not isinstance(y, int) or not 0 <= y < height:
            raise ValueError(f"{path}: point {index} y is outside canvas")

    adjacency = data.get("adjacency")
    if not isinstance(adjacency, list) or len(adjacency) != len(points):
        raise ValueError(f"{path}: adjacency must match point count")
    for point, neighbors in enumerate(adjacency):
        if not isinstance(neighbors, list):
            raise ValueError(f"{path}: adjacency {point} must be a list")
        if len(neighbors) > MAX_ADJACENCY_SLOTS:
            raise ValueError(f"{path}: adjacency {point} exceeds {MAX_ADJACENCY_SLOTS} slots")
        if len(neighbors) != len(set(neighbors)):
            raise ValueError(f"{path}: adjacency {point} contains duplicates")
        for neighbor in neighbors:
            if not isinstance(neighbor, int) or not 0 <= neighbor < len(points):
                raise ValueError(f"{path}: adjacency {point}->{neighbor} is invalid")
            if point not in adjacency[neighbor]:
                raise ValueError(f"{path}: adjacency {point}->{neighbor} is not bidirectional")

    mills = data.get("mills")
    if not isinstance(mills, list):
        raise ValueError(f"{path}: mills must be a list")
    if len(mills) > MAX_MILL_COUNT:
        raise ValueError(f"{path}: mill count exceeds {MAX_MILL_COUNT}")
    seen_mills = set()
    for mill in mills:
        if not isinstance(mill, list) or len(mill) != 3:
            raise ValueError(f"{path}: each mill must be three points")
        if len(set(mill)) != 3:
            raise ValueError(f"{path}: mill {mill} repeats a point")
        if any(not isinstance(point, int) or not 0 <= point < len(points) for point in mill):
            raise ValueError(f"{path}: mill {mill} references a missing point")
        key = tuple(sorted(mill))
        if key in seen_mills:
            raise ValueError(f"{path}: duplicate mill {mill}")
        seen_mills.add(key)

    rules = data.get("rules")
    if not isinstance(rules, dict):
        raise ValueError(f"{path}: rules must be an object")
    require_uint8(path, rules, "piecesPerPlayer")
    require_uint8(path, rules, "minPiecesToContinue")
    require_uint8(path, rules, "flyPieceCount")
    require_uint8(path, rules, "noCaptureDrawTurnLimit")
    require_uint8(path, rules, "placementStopEmptyPoints")
    require_mill_action(path, rules)
    require_bool(path, rules, "flyingEnabled")
    require_bool(path, rules, "mixedPlacementMovement")
    require_bool(path, rules, "protectPiecesInMills")
    require_bool(path, rules, "blockWinEnabled")
    require_bool(path, rules, "materialWinEnabled")
    require_bool(path, rules, "blockWinRequiresReserveEmpty")
    require_bool(path, rules, "materialWinRequiresReserveEmpty")
    require_bool(path, rules, "skipBlockedWithReserve")


def rule_flags_literal(rules: dict) -> str:
    flags = []
    if rules["millAction"] == "win":
        flags.append("RULE_MILL_ACTION_WIN")
    if rules["flyingEnabled"]:
        flags.append("RULE_FLYING_ENABLED")
    if rules["mixedPlacementMovement"]:
        flags.append("RULE_MIXED_PLACEMENT_MOVEMENT")
    if rules["protectPiecesInMills"]:
        flags.append("RULE_PROTECT_PIECES_IN_MILLS")
    if rules["blockWinEnabled"]:
        flags.append("RULE_BLOCK_WIN_ENABLED")
    if rules["materialWinEnabled"]:
        flags.append("RULE_MATERIAL_WIN_ENABLED")
    if rules["blockWinRequiresReserveEmpty"]:
        flags.append("RULE_BLOCK_WIN_REQUIRES_RESERVE_EMPTY")
    if rules["materialWinRequiresReserveEmpty"]:
        flags.append("RULE_MATERIAL_WIN_REQUIRES_RESERVE_EMPTY")
    if rules["skipBlockedWithReserve"]:
        flags.append("RULE_SKIP_BLOCKED_WITH_RESERVE")
    return " | ".join(flags) if flags else "0"


def format_points(profile: dict) -> str:
    rows = []
    for point in profile["points"]:
        rows.append(f"  {{{point['x']}, {point['y']}}},")
    return "\n".join(rows)


def format_mills(profile: dict) -> str:
    rows = []
    for mill in profile["mills"]:
        rows.append("  " + ", ".join(str(value) for value in mill) + ",")
    return "\n".join(rows)


def adjacency_offsets(profile: dict) -> list[int]:
    offsets = [0]
    for neighbors in profile["adjacency"]:
        offsets.append(offsets[-1] + len(neighbors))
    return offsets


def format_adjacency_offsets(profile: dict) -> str:
    return "  " + ", ".join(str(value) for value in adjacency_offsets(profile)) + ","


def format_adjacency(profile: dict) -> str:
    rows = []
    for neighbors in profile["adjacency"]:
        if neighbors:
            rows.append("  " + ", ".join(str(value) for value in neighbors) + ",")
    return "\n".join(rows) or "  "


def generated_header(profiles: list[dict]) -> str:
    board_externs = []
    rules_externs = []
    for profile in profiles:
        prefix = symbol_prefix(profile["id"])
        board_externs.append(f"extern const BoardPoint {prefix}BoardPoints[] PROGMEM;")
        board_externs.append(f"extern const uint8_t {prefix}Mills[] PROGMEM;")
        board_externs.append(f"extern const uint8_t {prefix}AdjacencyOffsets[] PROGMEM;")
        board_externs.append(f"extern const uint8_t {prefix}Adjacency[] PROGMEM;")
        board_externs.append(f"extern const BoardDefinition {prefix}BoardDefinition;")
        rules_externs.append(f"extern const RuleSet {prefix}RuleSet;")

    return "\n".join(
        [
            "#pragma once",
            "",
            "#include \"Board.h\"",
            "#include \"Rules.h\"",
            "",
            "// Generated by tools/board-data/generate.py. Do not edit by hand.",
            *board_externs,
            *rules_externs,
            "",
            f"constexpr uint8_t MORRIS_BOARD_PROFILE_COUNT = {len(profiles)};",
            "extern const BoardDefinition *const MorrisBoardProfiles[MORRIS_BOARD_PROFILE_COUNT] PROGMEM;",
            "extern const RuleSet *const MorrisRuleProfiles[MORRIS_BOARD_PROFILE_COUNT] PROGMEM;",
            "",
        ]
    )


def generated_source(profiles: list[dict]) -> str:
    lines = [
        "#include \"GeneratedBoards.h\"",
        "",
        "// Generated by tools/board-data/generate.py. Do not edit by hand.",
        "",
    ]

    for profile in profiles:
        prefix = symbol_prefix(profile["id"])
        rules = profile["rules"]
        lines.extend(
            [
                f"const BoardPoint {prefix}BoardPoints[] PROGMEM = {{",
                format_points(profile),
            ]
        )
        lines.extend(
            [
                "};",
                "",
                f"const uint8_t {prefix}Mills[] PROGMEM = {{",
                format_mills(profile),
                "};",
                "",
                f"const uint8_t {prefix}AdjacencyOffsets[] PROGMEM = {{",
                format_adjacency_offsets(profile),
                "};",
                "",
                f"const uint8_t {prefix}Adjacency[] PROGMEM = {{",
                format_adjacency(profile),
                "};",
                "",
                f"const BoardDefinition {prefix}BoardDefinition = {{",
                f"  \"{profile['label']}\",",
                f"  {len(profile['points'])},",
                f"  {len(profile['mills'])},",
                f"  {prefix}BoardPoints,",
                f"  {prefix}Mills,",
                f"  {prefix}AdjacencyOffsets,",
                f"  {prefix}Adjacency,",
                "};",
                "",
                f"const RuleSet {prefix}RuleSet = {{",
                f"  {rules['piecesPerPlayer']},",
                f"  {rules['minPiecesToContinue']},",
                f"  {rules['flyPieceCount']},",
                f"  {rules['noCaptureDrawTurnLimit']},",
                f"  {rules['placementStopEmptyPoints']},",
                f"  {rule_flags_literal(rules)},",
                "};",
                "",
            ]
        )

    board_refs = ", ".join(f"&{symbol_prefix(profile['id'])}BoardDefinition" for profile in profiles)
    rule_refs = ", ".join(f"&{symbol_prefix(profile['id'])}RuleSet" for profile in profiles)
    lines.extend(
        [
            f"const BoardDefinition *const MorrisBoardProfiles[MORRIS_BOARD_PROFILE_COUNT] PROGMEM = {{{board_refs}}};",
            f"const RuleSet *const MorrisRuleProfiles[MORRIS_BOARD_PROFILE_COUNT] PROGMEM = {{{rule_refs}}};",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--boards-dir", type=Path, default=ROOT / "boards")
    parser.add_argument("--out-dir", type=Path, default=ROOT / "src")
    args = parser.parse_args()

    paths = sorted(args.boards_dir.glob("*.json"))
    profiles = [load_profile(path) for path in paths]
    if not profiles:
        raise ValueError(f"{args.boards_dir}: no board profiles found")

    args.out_dir.mkdir(parents=True, exist_ok=True)
    (args.out_dir / "GeneratedBoards.h").write_text(generated_header(profiles), encoding="utf-8")
    (args.out_dir / "GeneratedBoards.cpp").write_text(generated_source(profiles), encoding="utf-8")
    print(f"Generated {len(profiles)} board profile(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
