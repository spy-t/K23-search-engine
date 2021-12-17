#include <core.h>
#include <qs/bk_tree.hpp>
#include <qs/entry.hpp>
#include <qs/hash_set.hpp>
#include <qs/hash_table.hpp>
#include <qs/memory.hpp>
#include <qs/parser.hpp>
#include <qs/vector.hpp>

struct Query {
  QueryID id;
  bool active;
  MatchType match_type;
  unsigned int match_dist;
  unsigned int word_count;

  Query(QueryID id, bool active, MatchType match_type, unsigned int match_dist,
        unsigned int word_count)
      : id(id), active(active), match_type(match_type), match_dist(match_dist),
        word_count(word_count) {}
};

static qs::hash_table<QueryID, qs::unique_pointer<Query>> queries;
struct QueryResult {
  Query *query;
  qs::hash_set<qs::string> matched_words;
};

struct DocumentResults {
  DocID docId;
  qs::hash_table<QueryID, QueryResult> results{};
};

struct DistanceThresholdCounters {
  int hamming;
  int edit;
};
using qvec = qs::vector<Query *>;
using entry = qs::entry<qvec>;

static qs::hash_table<qs::string, qvec> exact;

static qs::edit_dist<qvec> edit_functor;
static qs::hamming_dist<qvec> hamming_functor;

static qs::bk_tree<entry, qs::string> &edit_bk_tree() {
  static qs::bk_tree<entry, qs::string> edit_bk{&edit_functor};

  return edit_bk;
}

constexpr int HAMMING_BK_TREES = MAX_WORD_LENGTH - MIN_WORD_LENGTH;
static qs::bk_tree<entry, qs::string> *hamming_bk_trees() {
  static bool is_initialized = false;
  static qs::bk_tree<entry, qs::string> bk_trees[HAMMING_BK_TREES];
  if (!is_initialized) {
    for (auto &i : bk_trees) {
      i = qs::bk_tree<entry, qs::string>{&hamming_functor};
    }
    is_initialized = true;
  }

  return bk_trees;
}

// Always the last is the results of the active doc
static qs::vector<DocumentResults> results;
static qs::hash_table<unsigned int, DistanceThresholdCounters>
    thresholdCounters;
ErrorCode InitializeIndex() { return EC_SUCCESS; }

ErrorCode DestroyIndex() { return EC_SUCCESS; }

static void
add_query_to_doc_results(qs::hash_table<QueryID, QueryResult> &resultsTable,
                         Query *q, const qs::string &word) {
  auto iter = resultsTable.lookup(q->id);
  if (iter == resultsTable.end()) {
    auto queryRes = QueryResult{};
    queryRes.query = q;
    queryRes.matched_words.insert(word);
    resultsTable.insert(q->id, std::move(queryRes));
  } else {
    iter->matched_words.insert(word);
  }
}

static void match_queries(qs::bk_tree<entry, qs::string> &index,
                          const qs::string &w, DocumentResults &docRes,
                          MatchType match_type) {
  for (auto iter = thresholdCounters.begin(); iter != thresholdCounters.end();
       ++iter) {
    int d = 0;
    if (match_type == MT_EDIT_DIST) {
      d = iter->edit;
    } else if (match_type == MT_HAMMING_DIST) {
      d = iter->hamming;
    }
    if (d != 0) {
      auto matchedWords = index.match(iter.key(), w);
      for (auto &mw : matchedWords) {
        for (auto &mq : mw->payload) {
          if (mq->active) {
            add_query_to_doc_results(docRes.results, mq, mw->word);
          }
        }
      }
    }
  }
}

static void add_to_tree(Query *q, const qs::string &str,
                        qs::bk_tree<entry, qs::string> &tree) {
  auto found = tree.find(str);
  if (found.is_empty()) {
    auto en = entry(str);
    en.payload.push(q);
    tree.insert(en);
  } else {
    found.get()->payload.push(q);
  }
}

ErrorCode StartQuery(QueryID query_id, const char *query_str,
                     MatchType match_type, unsigned int match_dist) {
  auto q = qs::make_unique<Query>(query_id, true, match_type, match_dist, 0);
  auto unique_words = qs::hash_set<qs::string>();
  char *q_str = strdup(query_str);
  qs::parse_string(q_str, " ", [&q, &unique_words](qs::string &word) {
    auto &&place = unique_words.insert(word);
    if (place != unique_words.end()) {
      q->word_count++;
    }
  });
  free(q_str);

  if (match_type == MT_EDIT_DIST) {
    for (auto &str : unique_words) {
      add_to_tree(q.get(), str, edit_bk_tree());
    }
    auto iter = thresholdCounters.lookup(q->match_dist);
    if (iter == thresholdCounters.end()) {
      thresholdCounters.insert(q->match_dist, DistanceThresholdCounters{0, 1});
    } else {
      iter->edit++;
    }
  } else if (match_type == MT_HAMMING_DIST) {
    for (auto &str : unique_words) {
      add_to_tree(q.get(), str,
                  hamming_bk_trees()[str.length() - MIN_WORD_LENGTH]);
    }
    auto iter = thresholdCounters.lookup(q->match_dist);
    if (iter == thresholdCounters.end()) {
      thresholdCounters.insert(q->match_dist, DistanceThresholdCounters{1, 0});
    } else {
      iter->hamming++;
    }
  } else if (match_type == MT_EXACT_MATCH) {
    for (auto &str : unique_words) {
      auto f = exact.lookup(str);
      if (f == exact.end()) {
        auto qv = qvec{};
        qv.push(q.get());
        exact.insert(str, qv);
      } else {
        f->push(q.get());
      }
    }
  } else {
    return EC_FAIL;
  }
  queries.insert(std::move(query_id), std::move(q));
  return EC_SUCCESS;
}

ErrorCode EndQuery(QueryID query_id) {
  auto i = queries.lookup(query_id);
  if (i == queries.end()) {
    return EC_FAIL;
  }
  auto q = i->get();
  auto tC = thresholdCounters.lookup(q->match_dist);
  if (q->match_type == MT_EDIT_DIST) {
    tC->edit--;
  } else if (q->match_type == MT_HAMMING_DIST) {
    tC->hamming--;
  }
  q->active = false;
  return EC_SUCCESS;
}

ErrorCode MatchDocument(DocID doc_id, const char *doc_str) {
  char *q_str = strdup(doc_str);
  qs::hash_set<qs::string> dedu;
  qs::parse_string(q_str, " ", [&](qs::string &word) { dedu.insert(word); });
  auto docRes = DocumentResults{};
  docRes.docId = doc_id;
  free(q_str);
  for (auto &w : dedu) {
    // Check for edit distance
    auto &edit = edit_bk_tree();
    match_queries(edit, w, docRes, MT_EDIT_DIST);

    // Check for hamming distance
    auto hams = hamming_bk_trees();
    auto &ham = hams[w.length() - MIN_WORD_LENGTH];
    match_queries(ham, w, docRes, MT_HAMMING_DIST);

    // Check for exact match
    auto match = exact.lookup(w);
    if (match != exact.end()) {
      for (auto exactRes : *match) {
        if (exactRes->active) {
          add_query_to_doc_results(docRes.results, exactRes, w);
        }
      }
    }
  }
  results.push(std::move(docRes));
  return EC_SUCCESS;
}

int comp(const void *a, const void *b) { return *(QueryID *)a > *(QueryID *)b; }
ErrorCode GetNextAvailRes(DocID *p_doc_id, unsigned int *p_num_res,
                          QueryID **p_query_ids) {
  auto &docRes = results[results.get_size() - 1];
  *p_doc_id = docRes.docId;
  qs::vector<QueryID> res;
  int counter = 0;
  for (auto &qRes : docRes.results) {
    if (qRes.matched_words.get_size() == qRes.query->word_count) {
      counter++;
    }
  }
  *p_num_res = counter;
  *p_query_ids = new QueryID[counter];
  int i = 0;
  for (auto &qRes : docRes.results) {
    if (qRes.matched_words.get_size() == qRes.query->word_count) {
      (*p_query_ids)[i++] = qRes.query->id;
    }
  }
  qsort(*p_query_ids, counter, sizeof(QueryID), &comp);
  return EC_SUCCESS;
}
