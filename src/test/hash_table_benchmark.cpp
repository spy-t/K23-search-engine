#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch_amalgamated.hpp"

#include <qs/hash_table.hpp>
#include <qs/string.h>

void fill_and_lookup_sc(std::size_t max) {
  qs::hash_table<qs::string, int> sc;
  for (std::size_t i = 0; i < max; ++i) {
    sc.insert(qs::string(i), i);
  }

  for (std::size_t i = 0; i < max; ++i) {
    sc.lookup(qs::string(i));
  }

  for (std::size_t i = 0; i < max; ++i) {
    sc.remove(qs::string(i));
  }
}

void fill_and_lookup_oa(std::size_t max) {
  qs::oa_hash_table<qs::string, int> oa;
  for (std::size_t i = 0; i < max; ++i) {
    oa.insert(qs::string(i), i);
  }

  for (std::size_t i = 0; i < max; ++i) {
    oa.lookup(qs::string(i));
  }

  for (std::size_t i = 0; i < max; ++i) {
    oa.remove(qs::string(i));
  }
}

TEST_CASE("open addressing vs separate chaining benchmark", "[hash_table]") {
  BENCHMARK("separate chaining 10 keys") { fill_and_lookup_sc(10); };
  BENCHMARK("open addressing 10 keys") { fill_and_lookup_oa(10); };

  BENCHMARK("separate chaining 100 keys") { fill_and_lookup_sc(100); };
  BENCHMARK("open addressing 100 keys") { fill_and_lookup_oa(100); };

  BENCHMARK("separate chaining 1000 keys") { fill_and_lookup_sc(1000); };
  BENCHMARK("open addressing 1000 keys") { fill_and_lookup_oa(1000); };

  BENCHMARK("separate chaining 10000 keys") { fill_and_lookup_sc(10000); };
  BENCHMARK("open addressing 10000 keys") { fill_and_lookup_oa(10000); };
}
