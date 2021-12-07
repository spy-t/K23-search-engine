project('search', 'cpp',
	default_options : [
		'cpp_std=c++17', 
		'warning_level=3',
		'buildtype=debug'
	]
)

include = include_directories('include')

libqs_src = [
	'src/lib/bloom.cpp',
	'src/lib/distances.cpp',
	'src/lib/hash.cpp',
	'src/lib/sstream.cpp',
	'src/lib/string.cpp'
]

libqs_static = static_library('qs', libqs_src, include_directories : include)

# TODO(spyros): Turn this on when needed
# search_src = [
# 	'src/bin/search.cpp',
# 	'src/lib/fs.cpp'
# ]

# executable('search',
# 	sources : search_src,
# 	include_directories : include,
# 	override_options : ['warning_level=3'])

unit_test_sources = [
	'src/test/unit_main.cpp',
	'src/test/string_test.cpp',
	'src/test/sstream_test.cpp',
	'src/test/bk_tree_test.cpp',
	'src/test/vector_test.cpp',
	'src/test/distances_test.cpp',
	'src/test/unique_pointer_test.cpp',
	'src/test/hash_set_test.cpp',
	'src/test/list_test.cpp',
	'src/test/optional_test.cpp',
	'src/test/parser_test.cpp',
	'src/test/hash_table_test.cpp',
	'src/test/skip_list_test.cpp',
	'src/test/ops_test.cpp',
	'src/test/deduplication_test.cpp',
]

unit_tests = executable('unit_tests',
	sources : unit_test_sources,
	link_with : libqs_static,
	include_directories : include,
	cpp_args: '-DDEBUG'
)

test('unit_tests', unit_tests)

end_to_end_test_sources = [
	'src/test/query_test.cpp'
]

end_to_end_tests = executable('end_to_end_tests',
	sources : end_to_end_test_sources,
	link_with : libqs_static,
	include_directories : include,
	cpp_args: '-DDEBUG'
)

test('end_to_end_tests', end_to_end_tests)
