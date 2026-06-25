---
name: arduboy-deploy
description: Compile, package, and upload Arduboy projects. Use when Codex needs to build stable/debug Arduboy sketches, upload to a classic Arduboy over Arduino CLI, or prepare an Arduboy FX catalog entry without treating FX as a classic upload target.
---

# Arduboy Deploy

Use the project Makefile as the source of truth.

## Common Commands

Compile stable:

```sh
nix develop -c make compile
```

Compile debug:

```sh
nix develop -c make compile-debug
```

Prepare an Arduboy FX entry:

```sh
nix develop -c make fx-entry
```

## Safety Rules

- Never use classic Arduino upload for Arduboy FX catalog updates.
- Always back up an FX flashcart before replacing a catalog entry.
- Keep backups and generated images out of git.
- Require explicit confirmation before writing any full FX flashcart image.
- For FX-C catalog edits, do not patch slots by numeric guesses or rebuild from
  a decompiled folder without validating the resulting cart structure.

## FX-C Catalog Update Checklist

Before writing an edited FX-C flashcart image:

1. Back up every connected unit to `backups/fxc/`.
2. Verify the backups have the expected size and, when updating two equivalent
   units, compare their SHA-256 hashes.
3. Scan the backup with `ardugotools flashcart scan` and save the JSON output.
4. Record the category count, category names, total program count, and target
   category slot count.
5. Generate the candidate image from the backed-up `.bin`, not from a stale or
   previously corrupted working image.
6. Scan the candidate image with `ardugotools flashcart scan`.
7. Compare before/after JSON structurally:
   - category count must be unchanged;
   - category names and order must be unchanged;
   - every non-target category must keep the same slot titles in the same order;
   - the target category must change only by the intended add/update;
   - adjacent categories, especially the one after the target, must not merge or
     inherit the target game's banner/title.
8. Only write the candidate image if the structural comparison passes.
9. After writing, scan the device or a fresh backup and repeat the same category
   comparison against the candidate image.
