project('search', 'cpp',
	default_options : [
		'cpp_std=c++17',
		'warning_level=3',
		'buildtype=debug'
	]
)

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DQS_DEBUG', language: ['cpp'])
  add_project_arguments('-fkeep-inline-functions', language: ['cpp'])
  if get_option('heapprof') == true
    linkargs = ['-ltcmalloc', '-lprofiler']
  else
    linkargs = []
  endif
else
  linkargs = []
endif

include = include_directories('include')

threads_dep = dependency('threads')

###
# libqs
###

libqs_src = [
	'src/lib/bloom.cpp',
	'src/lib/distances.cpp',
	'src/lib/hash.cpp',
	'src/lib/sstream.cpp',
	'src/lib/string.cpp',
	'src/lib/string_view.cpp',
	'src/lib/scheduler.cpp'
	]

libqs_static = static_library('qs', libqs_src, include_directories : include, dependencies : threads_dep)

##
# libcore
##

libcore_shared = shared_library('core', 'src/core.cpp', include_directories: include, link_with: libqs_static)

core_test = executable('core_test',
    sources : [
      'src/test/test_through.cpp',
      'src/core.cpp'
    ],
    link_with : libqs_static,
    include_directories : include,
    link_args: linkargs
)

###
# Unit tests
###
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
	'src/test/string_view_test.cpp',
	'src/test/entry_test.cpp',
	'src/test/pair_test.cpp',
	'src/test/queue_test.cpp'
]

unit_tests = executable('unit_tests',
	sources : unit_test_sources,
	link_with : libqs_static,
	include_directories : include,
	cpp_args: '-DQS_DEBUG'
)

test('unit_tests', unit_tests)
