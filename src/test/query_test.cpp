#include "catch_amalgamated.hpp"

#include <qs/bk_tree.hpp>
#include <qs/distances.hpp>
#include <qs/list.hpp>
#include <qs/string.h>
#include <qs/vector.hpp>
#include <qs/functions.hpp>
#include <qs/hash_set.h>
#include <qs/parser.hpp>
#include <type_traits>
#include <qs/entry.hpp>

qs::bk_tree<qs::string> setUpWithoutEntryList(const char * queryFile,qs::distance_func<qs::string> dist){
  FILE *qf =  std::fopen(queryFile,"r");
  qs::bk_tree<qs::string> bk = qs::bk_tree(dist);
  qs::parse_file(qf, '\n', [&bk](const qs::string &entry) {
    bk.insert(entry);
  });
  fclose(qf);
  return bk;
}


SCENARIO("Query with medium query file and medium text"){
    const char *qpath = "../src/test/resources/med_query_test.txt";
    auto index = setUpWithoutEntryList(qpath,qs::edit_distance);
    const char *tpath = "../src/test/resources/med_text_test.txt";
    FILE * tf = std::fopen(tpath,"r");
    int threshold = 2;
      qs::parse_file(tf, '\n', [&index,&threshold](const qs::string &entry) {
        index.match(threshold,entry);
      });

    std::fclose(tf);
}