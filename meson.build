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
	'src/lib/hash_set.cpp',
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

tests = [
	'string_test',
	'sstream_test',
	'bk_tree_test',
	'vector_test',
	'distances_test',
	'unique_pointer_test',
	'hash_set_test',
	'list_test',
	'optional_test',
	'parser_test',
	'hash_table_test',
	'skip_list_test',
	'ops_test',
	'deduplication_test',
	'query_test'
]

global_test_deps = ['src/test/catch_amalgamated.cpp']

foreach test_name : tests
	t = executable(test_name,
		sources : ['src/test/@0@.cpp'.format(test_name)],
		link_with : libqs_static,
		include_directories : include,
		cpp_args: '-DDEBUG'
	)
	
	test(test_name, t)
endforeach

