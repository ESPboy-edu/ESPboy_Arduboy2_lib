# Arduboy FX Workflow

Do not use classic Arduino upload for an Arduboy FX catalog update.
Treat Arduboy FX-C as its own target/workflow: it uses the USB-C hardware and
link-cable capable platform, and its flashcart/update path should be explicit
instead of reusing the classic FX guard by accident.

FX and FX-C are not interchangeable deployment targets. Do not reuse a classic
FX backup image as the base for an FX-C write, and do not assume the same
preloaded catalog image, capacity, or update bundle. Always read a fresh backup
from the exact connected device, decompile that backup, add or replace only the
intended catalog entry, rebuild from that FX-C backup, and write the rebuilt
image back to that same class of device.

Safe flow:

1. Identify whether the connected unit is FX or FX-C.
2. Back up the current flashcart image from that exact unit.
3. Decompile the backup image.
4. Replace or add one explicit catalog entry with the new `.hex` and `.png`.
5. Rebuild the flashcart image.
6. Write the rebuilt image only after confirming the backup path.

This repo's `make upload` target intentionally refuses `TARGET=fx`. Use
`make fx-entry` to prepare files that can be merged into a backed-up catalog.
By default the entry is prepared in the `TableTop` category.

FX-C link-cable builds are prepared separately:

```sh
make compile-fxc
make fx-entry-fxc
```

`make fx-entry-fxc` writes under `dist/fx-cart/FX-C/TableTop/` so the result is
not accidentally merged into the classic FX catalog path.
