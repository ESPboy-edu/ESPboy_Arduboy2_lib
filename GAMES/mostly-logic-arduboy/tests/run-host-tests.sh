#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2026 MostlyBuilds

set -eu

build_dir=$(mktemp -d "${TMPDIR:-/tmp}/mostly-logic-tests.XXXXXX")
trap 'rm -rf "$build_dir"' EXIT HUP INT TERM

${CXX:-c++} -std=c++11 -Wall -Wextra -Werror \
  -I MostlyLogic \
  tests/test_game_types.cpp \
  -o "$build_dir/test_game_types"

${CXX:-c++} -std=c++11 -Wall -Wextra -Werror \
  -I tests/include -I MostlyLogic \
  tests/test_levels.cpp MostlyLogic/Levels.cpp \
  -o "$build_dir/test_levels"

"$build_dir/test_game_types"
"$build_dir/test_levels"
