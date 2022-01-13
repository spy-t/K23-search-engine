#include <core.h>
#include <qs/bk_tree.hpp>
#include <qs/entry.hpp>
#include <qs/hash_set.hpp>
#include <qs/hash_table.hpp>
#include <qs/memory.hpp>
#include <qs/parser.hpp>
#include <qs/scheduler.hpp>
#include <qs/string_view.h>
#include <qs/thread_safe_container.hpp>
#include <qs/vector.hpp>

#define THREADS_COUNT 4

struct Query {
  QueryID id;
  bool active;
  MatchType match_type;
  unsigned int match_dist;
  unsigned int word_count;
  qs::string query_str;
  qs::hash_set<qs::string_view> unique_words;

  Query(QueryID id, bool active, MatchType match_type, unsigned int match_dist,
        unsigned int word_count)
      : id(id), active(active), match_type(match_type), match_dist(match_dist),
        word_count(word_count) {}
};

static qs::hash_table<QueryID, qs::unique_pointer<Query>> queries;
struct QueryResult {
  Query *query;
  qs::hash_set<qs::string_view> matched_words;
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

// thread safe hash_table
using ts_hash_table =
    qs::thread_safe_container<qs::hash_table<qs::string_view, qvec>>;

static ts_hash_table &exact() {
  static ts_hash_table container{qs::hash_table<qs::string_view, qvec>{}};
  return container;
}

// thread safe bk_tree
using ts_bk_tree = qs::thread_safe_container<qs::bk_tree<entry>>;

static ts_bk_tree &edit_bk_tree() {
  static ts_bk_tree container{qs::bk_tree<entry>{&qs::edit_distance}};
  return container;
}

constexpr int HAMMING_BK_TREES = MAX_WORD_LENGTH - MIN_WORD_LENGTH;
static ts_bk_tree *hamming_bk_trees() {
  static bool is_initialized = false;
  static ts_bk_tree containers[HAMMING_BK_TREES];
  if (!is_initialized) {
    for (auto &i : containers) {
      i = qs::thread_safe_container<qs::bk_tree<entry>>{
          qs::bk_tree<entry>{&qs::hamming_distance}};
    }
    is_initialized = true;
  }
  return containers;
}

// Always the last is the results of the active doc
static qs::vector<DocumentResults> results;
static qs::hash_table<unsigned int, DistanceThresholdCounters>
    thresholdCounters;

ErrorCode InitializeIndex() { return EC_SUCCESS; }

ErrorCode DestroyIndex() { return EC_SUCCESS; }

static void
add_query_to_doc_results(qs::hash_table<QueryID, QueryResult> &resultsTable,
                         Query *q, qs::string_view &word) {
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

static void match_queries(ts_bk_tree &index, qs::string_view &w,
                          DocumentResults &docRes, MatchType match_type) {
  for (auto iter = thresholdCounters.begin(); iter != thresholdCounters.end();
       ++iter) {
    if ((match_type == MT_EDIT_DIST && iter->edit == 0) ||
        (match_type == MT_HAMMING_DIST && iter->hamming == 0)) {
      continue;
    }
    auto t = index.get_data();
    auto matchedWords = t->match((int)iter.key(), w);
    for (auto &mw : matchedWords) {
      for (auto mq : mw->payload) {
        if (mq->active && iter.key() == mq->match_dist) {
          add_query_to_doc_results(docRes.results, mq, mw->word);
        }
      }
    }
  }
}

static void add_to_tree(Query *q, qs::string_view *str, ts_bk_tree *tree) {
  auto t = tree->lock();
  if (t == nullptr) return;
  auto found = t->find(*str);
  if (found.is_empty()) {
    auto en = entry(*str);
    en.payload.push(q);
    t->insert(en);
  } else {
    found.get()->payload.push(q);
  }
  tree->unlock();
}

static void add_to_hash_table(Query *q, qs::string_view *str, ts_hash_table *ht) {
  auto e = ht->lock();
  if (e == nullptr) return;
  auto f = e->lookup(*str);
  if (f == e->end()) {
    auto qv = qvec{};
    qv.push(q);
    e->insert(*str, qv);
  } else {
    f->push(q);
  }
  ht->unlock();
}

static qs::scheduler &job_scheduler() {
  static qs::scheduler sched{THREADS_COUNT};
  return sched;
}

ErrorCode StartQuery(QueryID query_id, const char *query_str,
                     MatchType match_type, unsigned int match_dist) {
  auto q = qs::make_unique<Query>(query_id, true, match_type, match_dist, 0);
  q->query_str = qs::string{query_str};
  qs::parse_string(q->query_str.data(), ' ', [&q](qs::string_view &word) {
    auto &&place = q->unique_words.insert(word);
    if (place != q->unique_words.end()) {
      q->word_count++;
    }
  });

  if (match_type == MT_EDIT_DIST) {
    std::size_t i = 0;
    for (auto &str : q->unique_words) {
      job_scheduler().submit_job(add_to_tree, q.get(), &str, &edit_bk_tree());
      i++;
    }
    auto iter = thresholdCounters.lookup(q->match_dist);
    if (iter == thresholdCounters.end()) {
      thresholdCounters.insert(q->match_dist, DistanceThresholdCounters{0, 1});
    } else {
      iter->edit++;
    }
  } else if (match_type == MT_HAMMING_DIST) {
    std::size_t i = 0;
    for (auto &str : q->unique_words) {
      job_scheduler().submit_job(add_to_tree, q.get(), &str, &hamming_bk_trees()[str.size() - MIN_WORD_LENGTH]);
      i++;
    }
    auto iter = thresholdCounters.lookup(q->match_dist);
    if (iter == thresholdCounters.end()) {
      thresholdCounters.insert(q->match_dist, DistanceThresholdCounters{1, 0});
    } else {
      iter->hamming++;
    }

  } else if (match_type == MT_EXACT_MATCH) {
    std::size_t i = 0;
    for (auto &str : q->unique_words) {
      job_scheduler().submit_job(add_to_hash_table, q.get(), &str, &exact());
      i++;
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
  job_scheduler().wait_all_finish();
  qs::hash_set<qs::string_view> dedu;
  qs::parse_string(doc_str, ' ',
                   [&](qs::string_view &word) { dedu.insert(word); });
  auto docRes = DocumentResults{};
  docRes.docId = doc_id;
  for (auto &w : dedu) {
    // Check for edit distance
    auto &edit = edit_bk_tree();
    match_queries(edit, w, docRes, MT_EDIT_DIST);

    // Check for hamming distance
    auto hams = hamming_bk_trees();
    auto &ham = hams[w.size() - MIN_WORD_LENGTH];
    match_queries(ham, w, docRes, MT_HAMMING_DIST);

    // Check for exact match
    auto e = exact().get_data();
    auto match = e->lookup(w);
    if (match != e->end()) {
      for (auto exactRes : *match) {
        if (exactRes->active) {
          add_query_to_doc_results(docRes.results, exactRes, match.key());
        }
      }
    }
  }
  results.push(std::move(docRes));
  return EC_SUCCESS;
}

static size_t next_result = 0;
static int comp(const void *a, const void *b) {
  return *(QueryID *)a > *(QueryID *)b;
}
ErrorCode GetNextAvailRes(DocID *p_doc_id, unsigned int *p_num_res,
                          QueryID **p_query_ids) {
  if (next_result + 1 > results.get_size()) {
    return EC_FAIL;
  }
  auto &docRes = results[next_result++];
  *p_doc_id = docRes.docId;
  int counter = 0;
  for (auto &qRes : docRes.results) {
    if (qRes.matched_words.get_size() == qRes.query->word_count) {
      counter++;
    }
  }
  *p_num_res = counter;
  *p_query_ids = static_cast<QueryID *>(malloc(sizeof(QueryID) * counter));
  int i = 0;
  for (auto &qRes : docRes.results) {
    if (qRes.matched_words.get_size() == qRes.query->word_count) {
      (*p_query_ids)[i++] = qRes.query->id;
    }
  }
  qsort(*p_query_ids, counter, sizeof(QueryID), &comp);
  return EC_SUCCESS;
}
