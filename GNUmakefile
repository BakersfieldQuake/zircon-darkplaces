# GNUmakefile - convenience wrapper around the engine's own `makefile`.
#
# GNU make picks GNUmakefile before the lowercase `makefile`, so `make` runs this
# wrapper; the real build still lives in `makefile` and is invoked via -f below.
#
#   make            # build Linux client + dedicated server
#   make linux      # same
#   make windows    # build Windows (.exe) - run under MSYS2 MINGW64 or mingw cross env
#   make deps-linux # apt-get the Debian/Ubuntu build dependencies
#   make install    # copy the Linux binaries into the BAKOQUAKE package
#   make clean
#
# Linux links jpeg via dlopen (DP_LINK_JPEG=dlopen); without it the tree assumes
# the shared-link jpeg path but model_shared.c still references the dlopen jpeg_dll
# symbol and the link fails. Windows (mingw) uses the default shared jpeg link.

ENGINE_MAKEFILE := makefile
JOBS            := $(shell nproc 2>/dev/null || echo 4)

# Build outputs (names come from the engine makefile's EXE_UNIX*/EXE_WIN* vars).
LINUX_CLIENT    := zircon_beta_linux_sdl
LINUX_SERVER    := zircon_beta_linux_dedicated_server
WIN_CLIENT      := zircon_beta_windows_sdl.exe
WIN_SERVER      := zircon_beta_windows_dedicated_server.exe

# Where `make install` drops the Linux engine (override on the command line if needed).
BAKOQUAKE_DIR   := $(HOME)/Downloads/BAKOQUAKE

.PHONY: all linux windows deps-linux install clean help

all: linux

linux:
	$(MAKE) -f $(ENGINE_MAKEFILE) -j$(JOBS) DP_LINK_JPEG=dlopen sdl-release sv-release

windows:
	$(MAKE) -f $(ENGINE_MAKEFILE) -j$(JOBS) sdl-release sv-release

deps-linux:
	sudo apt-get update
	sudo apt-get install -y build-essential libsdl2-dev zlib1g-dev

# Zircon is a DarkPlaces fork; the BAKOQUAKE launchers look for these exact names.
install: linux
	install -m755 $(LINUX_CLIENT) $(BAKOQUAKE_DIR)/darkplaces-sdl
	install -m755 $(LINUX_SERVER) $(BAKOQUAKE_DIR)/darkplaces-dedicated
	@echo "Installed Linux engine into $(BAKOQUAKE_DIR)"

clean:
	$(MAKE) -f $(ENGINE_MAKEFILE) clean

help:
	@echo "Targets: all(=linux) | linux | windows | deps-linux | install | clean"
	@echo "Linux   : make deps-linux && make"
	@echo "Windows : (MSYS2 MINGW64) make windows"
	@echo "Install : make install   [BAKOQUAKE_DIR=/path/to/BAKOQUAKE]"
