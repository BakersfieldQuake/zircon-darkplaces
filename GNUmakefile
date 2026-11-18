# GNUmakefile - convenience wrapper around the engine's own `makefile`.
#
# GNU make reads GNUmakefile before the lowercase `makefile`, so `make` runs this
# wrapper. The engine makefile builds by re-invoking itself ($(MAKE) bin-release ...)
# WITHOUT -f, so those sub-makes also land here; the catch-all rule below forwards
# any target this wrapper does not define on to the engine makefile, which keeps the
# engine's internal recursion (bin-release, bin-debug, ...) working.
#
#   make            # build Linux client + dedicated server
#   make linux      # same
#   make windows    # build Windows (.exe) - run under MSYS2 MINGW64 or a mingw cross env
#   make deps-linux # apt-get the Debian/Ubuntu build dependencies
#   make clean      # forwarded to the engine makefile
#
# Linux links jpeg via dlopen (DP_LINK_JPEG=dlopen); without it the tree assumes the
# shared-link jpeg path but model_shared.c still references the dlopen jpeg_dll symbol
# and the link fails. Windows (mingw) uses the default shared jpeg link.

ENGINE := makefile
JOBS   := $(shell nproc 2>/dev/null || echo 4)

LINUX_CLIENT := zircon_beta_linux_sdl
LINUX_SERVER := zircon_beta_linux_dedicated_server
WIN_CLIENT   := zircon_beta_windows_sdl.exe
WIN_SERVER   := zircon_beta_windows_dedicated_server.exe

.PHONY: all linux windows deps-linux help

all: linux

linux:
	$(MAKE) -f $(ENGINE) -j$(JOBS) DP_LINK_JPEG=dlopen sdl-release sv-release

windows:
	$(MAKE) -f $(ENGINE) -j$(JOBS) sdl-release sv-release

deps-linux:
	sudo apt-get update
	sudo apt-get install -y build-essential libsdl2-dev zlib1g-dev

help:
	@echo "Targets: all(=linux) | linux | windows | deps-linux | clean"
	@echo "Linux   : make deps-linux && make   -> $(LINUX_CLIENT), $(LINUX_SERVER)"
	@echo "Windows : (MSYS2 MINGW64) make windows -> $(WIN_CLIENT), $(WIN_SERVER)"

# Don't let the catch-all try to remake the makefiles themselves.
GNUmakefile makefile makefile.inc: ;

# Forward everything else (the engine's recursive targets, clean, etc.) to the engine
# makefile. Command-line variables (EXE, LDFLAGS_COMMON, DP_MAKE_TARGET, ...) propagate
# to this sub-make automatically, so the engine's self-invocation keeps its arguments.
%:
	$(MAKE) -f $(ENGINE) $@
