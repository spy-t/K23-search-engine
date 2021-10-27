project('search', 'cpp', default_options : ['cpp_std=c++17', 'warning_level=3'])

include = include_directories('include')
src = [
	'src/bin/search.cpp',
	'src/lib/fs.cpp'
]

executable('search',
	sources : src,
	include_directories : include,
	override_options : ['warning_level=3'])

string_test = executable('string_test',
	sources : ['src/lib/string.cpp', 'src/test/string_test.cpp', 'src/test/catch_amalgamated.cpp'],
	include_directories : include
	)

test('string test', string_test)
