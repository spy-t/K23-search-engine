#define CATCH_CONFIG_RUNNER
#include "catch_amalgamated.hpp"

#include <iostream>
#include <string>

#include <qs/bk_tree.hpp>
#include <qs/distances.hpp>
#include <qs/entry.hpp>
#include <qs/functions.hpp>
#include <qs/hash_set.h>
#include <qs/list.hpp>
#include <qs/parser.hpp>
#include <qs/string.h>
#include <qs/vector.hpp>
#include <type_traits>

int threshold = -1;
std::string query_file;
std::string word_file;
std::string distance_function;
qs::distance_func<qs::string> dist;

int main(int argc, char *argv[]) {
  Catch::Session session;

  using namespace Catch::clara;
  auto cli =
      session.cli() |
      Opt(query_file,
          "query_file")["--queries"]("The file the queries will be read from") |
      Opt(word_file,
          "word_file")["--words"]("The file the queries will be read from") |
      Opt(threshold, "threshold")["--threshold"](
          "The threshold with which to perfrom the lookups") |
      Opt(distance_function, "distance_function")["--distance"](
          "The distance function that will be used for creating the bk_tree");

  session.cli(cli);
  int ret = session.applyCommandLine(argc, argv);
  if (ret != 0) {
    return ret;
  }

  if (threshold == -1) {
    return EXIT_SUCCESS;
  }

  if (distance_function == "edit") {
    dist = qs::edit_distance;
  } else if (distance_function == "hamming") {
    dist = qs::hamming_distance;
  } else {
    std::cout << "The distance function provided is not supported [edit, "
                 "hamming]\n";
    return EXIT_FAILURE;
  }

  return session.run();
}

SCENARIO("main") {
  FILE *qf = std::fopen(query_file.c_str(), "r");
  REQUIRE(qf != nullptr);

  qs::bk_tree<qs::string> bk = qs::bk_tree(dist);
  qs::parse_file(qf, '\n',
                 [&bk](const qs::string &entry) { bk.insert(entry); });
  fclose(qf);

  FILE *tf = std::fopen(word_file.c_str(), "r");
  qs::parse_file(tf, '\n',
                 [&](const qs::string &entry) { bk.match(threshold, entry); });

  std::fclose(tf);
}
