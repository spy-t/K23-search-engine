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

static qs::oa_hash_table<QueryID, qs::unique_pointer<Query>> queries;
struct QueryResult {
  Query *query;
  unsigned int word_found;
};

struct DocumentResults {
  DocID docId;
  qs::hash_table<QueryID, QueryResult> results;
};

struct DistanceThresholdCounters {
  int hamming;
  int edit;
};
using qvec = qs::vector<Query *>;
using entry = qs::entry<qvec>;

static qs::oa_hash_table<qs::string, qvec> exact;

static qs::edit_dist<qvec> edit_functor;
static qs::hamming_dist<qvec> hamming_functor;

static qs::bk_tree<entry> &edit_bk_tree() {
  static qs::bk_tree<entry> edit_bk{&edit_functor};

  return edit_bk;
}

constexpr int HAMMING_BK_TREES = MAX_WORD_LENGTH - MIN_WORD_LENGTH;
static qs::bk_tree<entry> *hamming_bk_trees() {
  static bool is_initialized = false;
  static qs::bk_tree<entry> bk_trees[HAMMING_BK_TREES];
  if (!is_initialized) {
    for (std::size_t i = 0; i < HAMMING_BK_TREES; ++i) {
      bk_trees[i] = qs::bk_tree<entry>{&hamming_functor};
    }
    is_initialized = true;
  }

  return bk_trees;
}

// Always the last is the results of the active doc
static qs::vector<DocumentResults *> results;
static qs::oa_hash_table<unsigned int, DistanceThresholdCounters>
    thresholdCounters;
ErrorCode InitializeIndex() { return EC_SUCCESS; }

ErrorCode DestroyIndex() { return EC_SUCCESS; }

static void add_to_tree(Query *q, entry &en, qs::bk_tree<entry> &tree) {
#ifdef DEBUG
  auto &qu = queries;
  auto &ex = exact;
  auto &ed = edit_bk_tree();
  auto h = hamming_bk_trees();
#endif
  auto found = tree.find(en);
  if (found.is_empty()) {
    en.payload.push(q);
    tree.insert(en);
  } else {
    found.get().payload.push(q);
  }
}

ErrorCode StartQuery(QueryID query_id, const char *query_str,
                     MatchType match_type, unsigned int match_dist) {
#ifdef DEBUG
  auto &qu = queries;
  auto &ex = exact;
  auto &ed = edit_bk_tree();
  auto h = hamming_bk_trees();
#endif
  auto q = qs::make_unique<Query>(query_id, true, match_type, match_dist, 0);
  auto unique_words = qs::hash_table<qs::string, entry>();
  char *q_str = strdup(query_str);
  qs::parse_string(q_str, " ", [&q, &unique_words](qs::string &word) {
    q->word_count++;
    unique_words.insert(word, entry(word));
  });
  free(q_str);

  if (match_type == MT_EDIT_DIST) {
    for (auto &en : unique_words) {
      add_to_tree(q.get(), en, edit_bk_tree());
    }
    auto iter = thresholdCounters.lookup(q->match_dist);
    if (iter == thresholdCounters.end()) {
      thresholdCounters.insert(q->match_dist, DistanceThresholdCounters{0, 1});
    } else {
      iter->edit++;
    }
  } else if (match_type == MT_HAMMING_DIST) {
    for (auto &en : unique_words) {
      add_to_tree(q.get(), en,
                  hamming_bk_trees()[en.word.length() - MIN_WORD_LENGTH]);
    }
    auto iter = thresholdCounters.lookup(q->match_dist);
    if (iter == thresholdCounters.end()) {
      thresholdCounters.insert(q->match_dist, DistanceThresholdCounters{1, 0});
    } else {
      iter->hamming++;
    }
  } else if (match_type == MT_EXACT_MATCH) {
    for (auto &en : unique_words) {
      auto f = exact.lookup(en.word);
      if (f == exact.end()) {
        auto qv = qvec{};
        qv.push(q.get());
        exact.insert(en.word, std::move(qv));
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
#ifdef DEBUG
  auto &qu = queries;
  auto &ex = exact;
  auto &ed = edit_bk_tree();
  auto h = hamming_bk_trees();
#endif
  auto i = queries.lookup(query_id);
  if (i == queries.end()) {
    return EC_FAIL;
  }
  auto q = (*i);
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
  free(q_str);

  for (auto &w : dedu) {
    // we need max 3 loops for edit as many as the unique thresholds
    // Check for edit distance
    // Check for hamming distance

    // Check for exact match
    auto match = exact.lookup(w);
    if (match != exact.end()) {
      for (auto exactRes : *match) {
        if (exactRes->active) {
          auto iter = docRes.results.lookup(exactRes->id);
          if (iter == docRes.results.end()) {
            auto queryRes = QueryResult{};
            queryRes.query = exactRes;
            queryRes.word_found = 1;
            docRes.results.insert(exactRes->id, queryRes);
          } else {
            iter->word_found++;
          }
        }
      }
    }
  }
  return EC_SUCCESS;
}

ErrorCode GetNextAvailRes(DocID *p_doc_id, unsigned int *p_num_res,
                          QueryID **p_query_ids) {
#ifdef DEBUG
  auto &qu = queries;
  auto &ex = exact;
  auto &ed = edit_bk_tree();
  auto h = hamming_bk_trees();
#endif
  return EC_SUCCESS;
}

