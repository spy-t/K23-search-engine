#include <core.h>
#include <qs/bk_tree.hpp>
#include <qs/entry.hpp>
#include <qs/hash_set.hpp>
#include <qs/hash_table.hpp>
#include <qs/job.h>
#include <qs/memory.hpp>
#include <qs/parser.hpp>
#include <qs/scheduler.hpp>
#include <qs/string_view.h>
#include <qs/thread_safe_container.hpp>
#include <qs/vector.hpp>

#define DEFAULT_THREADS_COUNT 16

struct Query {
  QueryID id;
  bool active;
  MatchType match_type;
  unsigned int match_dist;
  unsigned int word_count;
  qs::string query_str;
  qs::hash_set<qs::string_view> unique_words{MAX_QUERY_WORDS};

  Query(QueryID id, bool active, MatchType match_type, unsigned int match_dist,
        unsigned int word_count)
      : id(id), active(active), match_type(match_type), match_dist(match_dist),
        word_count(word_count) {}
};

static qs::hash_table<QueryID, qs::unique_pointer<Query>> queries{4096};
struct QueryResult {
  Query *query;
  qs::hash_set<qs::string_view> matched_words;
};

struct DocumentResults {
  DocID docId{};
  qs::thread_safe_container<qs::hash_table<QueryID, QueryResult>> results;
  qs::hash_set<qs::string_view> words;
  qs::string doc_str;

  DocumentResults() = default;
  DocumentResults(DocID docId, size_t results_cap, const char *doc_str)
      : docId{docId}, results{qs::hash_table<QueryID, QueryResult>{
                          results_cap}},
        doc_str(doc_str) {}
  DocumentResults(DocumentResults &&other) noexcept
      : docId{other.docId}, results{std::move(other.results)},
        words{std::move(other.words)}, doc_str(std::move(other.doc_str)) {}
  DocumentResults(const DocumentResults &other) = delete;
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
  static ts_hash_table container{qs::hash_table<qs::string_view, qvec>{4096}};
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
static qs::queue<DocumentResults> results;
static qs::hash_table<unsigned int, DistanceThresholdCounters>
    thresholdCounters{128};

ErrorCode InitializeIndex() { return EC_SUCCESS; }

ErrorCode DestroyIndex() { return EC_SUCCESS; }

static void *add_to_tree(Query *q, qs::string_view *str, ts_bk_tree *tree) {
  auto t = tree->lock();
  if (t == nullptr)
    return nullptr;
  auto found = t->find(*str);
  if (found == nullptr) {
    auto en = entry(*str);
    en.payload.push(q);
    t->insert(en);
  } else {
    found->payload.push(q);
  }
  tree->unlock();
  return nullptr;
}

struct add_to_tree_job : public qs::job {
  Query *q;
  qs::string_view *str;
  ts_bk_tree *tree;

  add_to_tree_job(Query *q, qs::string_view *str, ts_bk_tree *tree)
      : q{q}, str{str}, tree{tree} {}

  void operator()() override { add_to_tree(q, str, tree); }
};

static void *add_to_hash_table(Query *q, qs::string_view *str,
                               ts_hash_table *ht) {

  auto e = ht->lock();
  if (e == nullptr)
    return nullptr;
  auto f = e->lookup(*str);
  if (f == e->end()) {
    auto qv = qvec{};
    qv.push(q);
    e->insert(*str, qv);
  } else {
    f->push(q);
  }
  ht->unlock();
  return nullptr;
}

struct add_to_hash_table_job : public qs::job {
  Query *q;
  qs::string_view *str;
  ts_hash_table *ht;

  add_to_hash_table_job(Query *q, qs::string_view *str, ts_hash_table *ht)
      : q{q}, str{str}, ht{ht} {}

  void operator()() override { add_to_hash_table(q, str, ht); }
};

static qs::scheduler &job_scheduler() {
  static bool scheduler_initialized = false;
  static u32 threads = DEFAULT_THREADS_COUNT;
  if (!scheduler_initialized) {
    const char *search_threads = std::getenv("SEARCH_THREADS");
    if (search_threads && std::strlen(search_threads)) {
      threads = std::atoi(search_threads);
      if (!threads) {
        threads = DEFAULT_THREADS_COUNT;
      }
    }
  }
  static qs::scheduler sched{threads};
  return sched;
}

bool query_has_started = false;

std::size_t active_queries = 0;
ErrorCode StartQuery(QueryID query_id, const char *query_str,
                     MatchType match_type, unsigned int match_dist) {
  active_queries++;
  query_has_started = true;
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
      job_scheduler().submit_job(
          new add_to_tree_job{q.get(), &str, &edit_bk_tree()});
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
      job_scheduler().submit_job(new add_to_tree_job{
          q.get(), &str, &hamming_bk_trees()[str.size() - MIN_WORD_LENGTH]});
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
      job_scheduler().submit_job(
          new add_to_hash_table_job{q.get(), &str, &exact()});
      i++;
    }
  } else {
    return EC_FAIL;
  }
  queries.insert(std::move(query_id), std::move(q));
  return EC_SUCCESS;
}

ErrorCode EndQuery(QueryID query_id) {
  active_queries--;
  job_scheduler().wait_all_finish();
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

static void add_query_to_doc_results(
    qs::thread_safe_container<qs::hash_table<QueryID, QueryResult>>
        &resultsTable,
    Query *q, qs::string_view *word) {
  auto rt = resultsTable.lock();
  auto iter = rt->lookup(q->id);
  if (iter == rt->end()) {
    auto queryRes = QueryResult{};
    queryRes.query = q;
    queryRes.matched_words.insert(*word);
    rt->insert(q->id, std::move(queryRes));
  } else {
    iter->matched_words.insert(*word);
  }
  resultsTable.unlock();
}

static void *match_queries(ts_bk_tree *index, qs::string_view *w,
                           DocumentResults *docRes, MatchType match_type) {
  for (auto iter = thresholdCounters.begin(); iter != thresholdCounters.end();
       ++iter) {
    if ((match_type == MT_EDIT_DIST && iter->edit == 0) ||
        (match_type == MT_HAMMING_DIST && iter->hamming == 0)) {
      continue;
    }
    auto t = index->get_data();
    auto matchedWords = t->match((int)iter.key(), *w);
    for (auto &mw : matchedWords) {
      for (auto mq : mw->payload) {
        if (mq->active && iter.key() == mq->match_dist) {
          add_query_to_doc_results(docRes->results, mq, &mw->word);
        }
      }
    }
  }
  return nullptr;
}

struct match_queries_job : public qs::job {

  ts_bk_tree *index;
  qs::string_view *w;
  DocumentResults *docRes;
  MatchType match_type;

  match_queries_job(ts_bk_tree *index, qs::string_view *w,
                    DocumentResults *docRes, MatchType match_type)
      : index{index}, w{w}, docRes{docRes}, match_type{match_type} {}

  void operator()() override { match_queries(index, w, docRes, match_type); }
};

static void *match_exact(ts_hash_table *e, qs::string_view *w,
                         DocumentResults *docRes) {
  auto ht = e->get_data();
  auto match = ht->lookup(*w);
  if (match != ht->end()) {
    for (auto exactRes : *match) {
      if (exactRes->active) {
        add_query_to_doc_results(docRes->results, exactRes, &match.key());
      }
    }
  }
  return nullptr;
}

struct match_exact_job : public qs::job {
  ts_hash_table *e;
  qs::string_view *w;
  DocumentResults *docRes;

  match_exact_job(ts_hash_table *e, qs::string_view *w, DocumentResults *docRes)
      : e{e}, w{w}, docRes{docRes} {}

  void operator()() override { match_exact(e, w, docRes); }
};

bool match_has_started = false;

ErrorCode MatchDocument(DocID doc_id, const char *doc_str) {
  if (query_has_started) {
    job_scheduler().wait_all_finish();
    query_has_started = false;
  }
  match_has_started = true;
  results.enqueue(DocumentResults{doc_id, active_queries, doc_str});
  auto &docRes = *(results.last());
  qs::parse_string(docRes.doc_str.data(), ' ',
                   [&](qs::string_view &word) { docRes.words.insert(word); });
  for (auto &w : docRes.words) {
    // Check for edit distance
    auto &edit = edit_bk_tree();
    job_scheduler().submit_job(
        new match_queries_job{&edit, &w, &docRes, MT_EDIT_DIST});

    // Check for hamming distance
    auto hams = hamming_bk_trees();
    auto &ham = hams[w.size() - MIN_WORD_LENGTH];
    job_scheduler().submit_job(
        new match_queries_job{&ham, &w, &docRes, MT_HAMMING_DIST});

    // Check for exact match
    job_scheduler().submit_job(new match_exact_job{&exact(), &w, &docRes});
  }
  return EC_SUCCESS;
}

static int comp(const void *a, const void *b) {
  return *(QueryID *)a > *(QueryID *)b;
}
ErrorCode GetNextAvailRes(DocID *p_doc_id, unsigned int *p_num_res,
                          QueryID **p_query_ids) {
  job_scheduler().wait_all_finish();
  auto opt = results.dequeue();
  if (opt.is_empty()) {
    return EC_NO_AVAIL_RES;
  }
  auto &docRes = opt.get();
  *p_doc_id = docRes.docId;
  *p_num_res = 0;

  *p_query_ids =
      static_cast<QueryID *>(malloc(sizeof(QueryID) * queries.get_size()));

  auto r = docRes.results.get_data();
  for (auto &qRes : (*r)) {
    if (qRes.matched_words.get_size() == qRes.query->word_count) {
      (*p_query_ids)[(*p_num_res)++] = qRes.query->id;
    }
  }

  qsort(*p_query_ids, *p_num_res, sizeof(QueryID), &comp);
  return EC_SUCCESS;
}
