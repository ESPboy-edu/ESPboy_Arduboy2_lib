ARDUINO_CLI ?= arduino-cli
PYTHON ?= python3
FQBN ?= arduino:avr:leonardo
SKETCH_DIR := $(CURDIR)
BUILD ?= stable
BUILD_DIR ?= $(SKETCH_DIR)/build/$(BUILD)
DIST_DIR ?= $(SKETCH_DIR)/dist
ARDUINO_DATA_DIR ?= $(SKETCH_DIR)/.arduino
PORT ?=
TARGET ?= fx
OPEN ?= xdg-open
RETROARCH ?= retroarch
ARDENS_LIBRETRO_CORE ?=
RETROARCH_CONFIG ?= $(SKETCH_DIR)/config/retroarch/arduboy-clean.cfg
FX_CART_DIR ?= $(DIST_DIR)/fx-cart
FX_CATEGORY ?= TableTop
FX_GAME ?= 02-All-Mens-Morris
FX_BANNER ?= assets/fx/banner.png
ARDUBOY_VERSION ?= dev
ARDUBOY_PACKAGE_SCRIPT ?= $(SKETCH_DIR)/tools/package-arduboy.py
ARDUBOY_PACKAGE := $(DIST_DIR)/release/all-mens-morris-$(ARDUBOY_VERSION).arduboy

ELF := $(BUILD_DIR)/all-mens-morris.ino.elf
HEX := $(BUILD_DIR)/all-mens-morris.ino.hex
FXC_HEX := $(SKETCH_DIR)/build/fxc/all-mens-morris.ino.hex
AVR_SIZE ?= avr-size
AVR_NM ?= avr-nm
ARDUINO_SIZE_FLAGS := --build-property compiler.c.extra_flags="-mcall-prologues -fno-inline-small-functions" --build-property compiler.cpp.extra_flags="-mcall-prologues -fno-inline-small-functions" --build-property compiler.c.elf.extra_flags="-Wl,--relax"

ifeq ($(BUILD),debug)
ARDUINO_BUILD_FLAGS := --build-property compiler.cpp.extra_flags="-DALL_MENS_MORRIS_DEBUG=1 -mcall-prologues -fno-inline-small-functions" --build-property compiler.c.extra_flags="-mcall-prologues -fno-inline-small-functions" --build-property compiler.c.elf.extra_flags="-Wl,--relax"
else ifeq ($(BUILD),fxc)
ARDUINO_BUILD_FLAGS := --build-property compiler.cpp.extra_flags="-DALL_MENS_MORRIS_FXC_LINK=1 -mcall-prologues -fno-inline-small-functions" --build-property compiler.c.extra_flags="-mcall-prologues -fno-inline-small-functions" --build-property compiler.c.elf.extra_flags="-Wl,--relax"
else ifeq ($(BUILD),stable)
ARDUINO_BUILD_FLAGS := $(ARDUINO_SIZE_FLAGS)
else
$(error BUILD must be stable, debug, or fxc)
endif

export ARDUINO_DIRECTORIES_DATA := $(ARDUINO_DATA_DIR)
export ARDUINO_DIRECTORIES_USER := $(SKETCH_DIR)/.arduino-sketchbook

.PHONY: all setup compile compile-debug compile-fxc upload upload-sketch clean hex size size-debug symbols symbols-debug check sim cloud libretro libretro-debug fx-entry fx-entry-fxc package-arduboy tabletop-studio board-data music-data

all: compile

setup:
	$(ARDUINO_CLI) core update-index
	$(ARDUINO_CLI) core install arduino:avr
	$(ARDUINO_CLI) lib install Arduboy2 Arduboy-TinyFont ArduboyTones
	rm -rf "$(SKETCH_DIR)/.arduino-sketchbook/libraries/ArduboyI2C"
	ARDUINO_LIBRARY_ENABLE_UNSAFE_INSTALL=true $(ARDUINO_CLI) lib install --git-url https://github.com/sub1inear/ArduboyI2C.git#74d9e8d89111e6b19292d5c6e1ac576710137c5d

compile:
	$(ARDUINO_CLI) compile --fqbn $(FQBN) $(ARDUINO_BUILD_FLAGS) --build-path $(BUILD_DIR) $(SKETCH_DIR)

compile-debug:
	$(MAKE) compile BUILD=debug

compile-fxc:
	$(MAKE) compile BUILD=fxc

upload:
ifeq ($(TARGET),fx)
	$(error TARGET=fx protects your Arduboy FX catalog. Use make fx-entry, then merge that entry into a backed-up flashcart image)
endif
	$(MAKE) upload-sketch TARGET=$(TARGET) PORT=$(PORT) CONFIRM_OVERWRITE=$(CONFIRM_OVERWRITE)

upload-sketch:
ifneq ($(CONFIRM_OVERWRITE),1)
	$(error Refusing to overwrite the main sketch. Re-run with CONFIRM_OVERWRITE=1 after confirming this is not an FX catalog update)
endif
ifndef PORT
	$(error Set PORT=/dev/ttyACM0 or another Arduboy serial device)
endif
	$(ARDUINO_CLI) upload --fqbn $(FQBN) --port $(PORT) $(SKETCH_DIR)

hex: compile
	@printf '%s\n' "$(HEX)"

size: compile
	$(AVR_SIZE) -C --mcu=atmega32u4 "$(ELF)"
	$(AVR_SIZE) -A "$(ELF)"

size-debug:
	$(MAKE) size BUILD=debug

symbols: compile
	$(AVR_NM) --print-size --size-sort --radix=d "$(ELF)"

symbols-debug:
	$(MAKE) symbols BUILD=debug

sim: compile
	@printf '%s\n' "Open this HEX in Arduboy Cloud, Ardens, or another Arduboy emulator:"
	@printf '%s\n' "$(HEX)"

cloud: compile
	@printf '%s\n' "Opening Arduboy Cloud. Load this HEX in the emulator:"
	@printf '%s\n' "$(HEX)"
	$(OPEN) "https://cloud.arduboy.com/"

libretro: compile
ifndef ARDENS_LIBRETRO_CORE
	$(error Enter nix develop first, or set ARDENS_LIBRETRO_CORE=/path/to/ardens_libretro.so)
endif
	$(RETROARCH) --appendconfig="$(RETROARCH_CONFIG)" --set-shader="" -L "$(ARDENS_LIBRETRO_CORE)" "$(HEX)"

libretro-debug:
	$(MAKE) libretro BUILD=debug

fx-entry: compile
	install -D "$(HEX)" "$(FX_CART_DIR)/$(FX_CATEGORY)/$(FX_GAME).hex"
	install -D "$(SKETCH_DIR)/$(FX_BANNER)" "$(FX_CART_DIR)/$(FX_CATEGORY)/$(FX_GAME).png"
	@printf '%s\n' "FX catalog entry prepared:"
	@printf '%s\n' "$(FX_CART_DIR)/$(FX_CATEGORY)/$(FX_GAME).hex"
	@printf '%s\n' "$(FX_CART_DIR)/$(FX_CATEGORY)/$(FX_GAME).png"
	@printf '%s\n' "Merge this into a backup of your existing FX cart before writing."

fx-entry-fxc: compile-fxc
	install -D "$(FXC_HEX)" "$(FX_CART_DIR)/FX-C/$(FX_CATEGORY)/$(FX_GAME).hex"
	install -D "$(SKETCH_DIR)/$(FX_BANNER)" "$(FX_CART_DIR)/FX-C/$(FX_CATEGORY)/$(FX_GAME).png"
	@printf '%s\n' "FX-C catalog entry prepared:"
	@printf '%s\n' "$(FX_CART_DIR)/FX-C/$(FX_CATEGORY)/$(FX_GAME).hex"
	@printf '%s\n' "$(FX_CART_DIR)/FX-C/$(FX_CATEGORY)/$(FX_GAME).png"
	@printf '%s\n' "Merge this into an FX-C flashcart backup, not a classic FX cart."

package-arduboy: compile
	$(PYTHON) "$(ARDUBOY_PACKAGE_SCRIPT)" \
		--hex "$(HEX)" \
		--banner "$(SKETCH_DIR)/$(FX_BANNER)" \
		--output "$(ARDUBOY_PACKAGE)" \
		--version "$(ARDUBOY_VERSION)"
	@printf '%s\n' "Arduboy package prepared:"
	@printf '%s\n' "$(ARDUBOY_PACKAGE)"

tabletop-studio:
	$(PYTHON) tools/tabletop-studio/server.py --open

board-data:
	$(PYTHON) tools/board-data/generate.py

music-data:
	$(PYTHON) tools/music/generate_menu_music.py

check: board-data music-data compile

clean:
	rm -rf "$(BUILD_DIR)" "$(DIST_DIR)"
