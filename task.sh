#! /usr/bin/env bash

set -eu -o pipefail

# find the root and cd to it so all the commands can be run from any subdirectory
ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

function init {
	meson builddir
	ln -s builddir/compile_commands.json ./
}

function compile {
	cd builddir && meson compile
}

function test {
	cd builddir && meson test --print-errorlogs
}

function test-mem {
	cd builddir && meson test --print-errorlogs --wrap='valgrind --leak-check=full'
}

function format {
	clang-format -i $(find -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp")
}

"$@"
