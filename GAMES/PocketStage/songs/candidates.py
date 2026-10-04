# The songs Joel picks the first setlist from (2026-10-02): numbered in this
# order on the page tools/songs_page.py makes, from the easiest to the hardest
# (by how many notes a second Expert has), in tiers of ten. FAVOURITES are
# fifteen suggested beforehand, three a tier; IN_THE_GAME the two the game had
# then.

CANDIDATES = [
    # warm-up
    "dead_pixel", "gravity", "heavy_rain", "save_state", "grey_sky",
    "low_power", "candlelight", "no_grey", "dune_buggy", "stage_dive",
    # club
    "glitter", "idle_hands", "street_dog", "cascade", "hot_wired",
    "boogie_loop", "quick_fix", "overdrive", "seven_suns", "no_signal",
    # big stage
    "segfault", "undefined", "sky_rider", "fix_me_later", "signal_fire",
    "go_go_go", "offbeat", "bored_again", "fatal_error", "neon_fever",
    # arena
    "breakpoint", "high_score", "duckwalk", "hot_plate", "slapback",
    "pipe_dream", "hard_reset", "no_brakes", "night_driver", "texas_heat",
    # encore
    "shark_bait", "deadline", "warhorse", "escape_key", "skyforge",
    "brute_force", "hot_fingers", "xyzzy", "zero_day", "hell_mode",
]

FAVOURITES = [
    "dead_pixel", "no_grey", "stage_dive",
    "hot_wired", "overdrive", "seven_suns",
    "signal_fire", "offbeat", "fatal_error",
    "high_score", "night_driver", "texas_heat",
    "warhorse", "escape_key", "skyforge",
]
IN_THE_GAME = ["overdrive", "stage_dive"]
PICK = 15
TIERS = [("Warm-up", 10), ("Club", 10), ("Big stage", 10), ("Arena", 10), ("Encore", 10)]
