#define CATCH_CONFIG_RUNNER
#include "catch_amalgamated.hpp"

#include <iostream>
#include <string>

#include <qs/bk_tree.hpp>
#include <qs/entry.hpp>
#include <qs/hash_set.hpp>
#include <qs/parser.hpp>
#include <qs/string.h>
#include <type_traits>

int threshold = -1;
std::string query_file;
std::string word_file;
std::string distance_function;
qs::distance_func<qs::entry<char>> *dist;

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

  if (distance_function == "edit") {
    dist = new qs::edit_dist<char>();
  } else if (distance_function == "hamming") {
    dist = new qs::hamming_dist<char>();
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
  qs::hash_set<qs::string> set;
  qs::parse_file(qf, '\n',
                 [&set](const qs::string &entry) { set.insert(entry); });
  fclose(qf);
  auto entry_list = qs::skip_list<qs::entry<char>, 16>(
      [](const qs::entry<char> &a, const qs::entry<char> &b) {
        if (a.word == b.word) {
          return 0;
        }

        return a.word > b.word ? -1 : 1;
      });

  for (auto &e : set) {
    entry_list.insert(qs::entry(e, '\0'));
  }
  qs::bk_tree<qs::entry<char>> bk(entry_list, dist);

  FILE *tf = std::fopen(word_file.c_str(), "r");
  REQUIRE(tf != nullptr);
  qs::parse_file(tf, '\n', [&](const qs::string &entry) {
    bk.match(threshold, qs::entry(entry, '\0'));
  });

  delete dist;
  std::fclose(tf);
}
