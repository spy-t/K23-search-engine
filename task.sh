#! /usr/bin/env bash

set -eu -o pipefail

# find the root and cd to it so all the commands can be run from any subdirectory
ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

function init {
	meson build
	ln -s build/compile_commands.json ./
}

function compile {
	meson compile -C build
}

function test {
	meson test "$@" -C build --print-errorlogs
}

function test-mem {
	meson test "$@" -C build --print-errorlogs --wrap='valgrind --leak-check=full'
}

function coverage {
	ninja -C build coverage
}

function format {
	ninja -C build clang-format
}

"$@"
