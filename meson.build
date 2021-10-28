project('search', 'cpp',
	default_options : [
		'cpp_std=c++17', 
		'warning_level=3',
		'buildtype=debug'
	]
)

include = include_directories('include')

# TODO(spyros): Turn this on when needed
# search_src = [
# 	'src/bin/search.cpp',
# 	'src/lib/fs.cpp'
# ]

# executable('search',
# 	sources : search_src,
# 	include_directories : include,
# 	override_options : ['warning_level=3'])


add_test_setup('valgrind',
	exe_wrapper: ['valgrind', '--leak-check=full', '--track-origins=yes', '--exit-on-first-error=yes'],
	is_default: false)

tests = {
	'string_test': ['src/lib/string.cpp'],
	'sstream_test': ['src/lib/sstream.cpp', 'src/lib/string.cpp']
}
global_test_deps = ['src/test/catch_amalgamated.cpp']

foreach test_name, test_deps : tests
	t = executable(test_name,
		sources : [f'src/test/@test_name@.cpp'] + test_deps + global_test_deps,
		include_directories : include
	)
	
	test(test_name, t)
endforeach

