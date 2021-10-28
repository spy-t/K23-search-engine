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
	cd build && meson compile
}

function test {
	cd build && meson test "$@" --print-errorlogs
}

function test-mem {
	cd build && meson test "$@" --print-errorlogs --wrap='valgrind --leak-check=full'
}

function coverage {
	cd build && ninja coverage
}

function format {
	clang-format -i $(find -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp")
}

"$@"
