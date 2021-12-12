#include <core.h>
#include <qs/entry.hpp>
#include <qs/hash_table.hpp>
#include <qs/vector.hpp>
#include <qs/bk_tree.hpp>
#include <qs/parser.hpp>

struct Query {
  QueryID id;
  bool active;
  MatchType match_type;
  unsigned int match_dist;
  unsigned int word_count;
};

qs::hash_table<QueryID , Query*> *queries;

using entry = qs::entry<qs::vector<Query *>*>;

qs::hash_table<qs::string , qs::vector<Query *>> *exact;
qs::bk_tree<entry> *edit;
qs::bk_tree<entry> hamming[MAX_WORD_LENGTH - MIN_WORD_LENGTH];

qs::edit_dist<qs::vector<Query *>*> *edit_functor;
qs::hamming_dist<qs::vector<Query *>*> *hamming_functor;

ErrorCode InitializeIndex() {
  queries = new qs::hash_table<QueryID , Query *>();
  exact = new qs::hash_table<qs::string , qs::vector<Query *>>();
  edit_functor = new qs::edit_dist<qs::vector<Query *>*>();
  hamming_functor = new qs::hamming_dist<qs::vector<Query *>*>();
  edit = new qs::bk_tree<entry>(edit_functor);
  for (auto & i : hamming) {
    i = qs::bk_tree<entry>(hamming_functor);
  }
  return EC_SUCCESS;
}

ErrorCode DestroyIndex() {
  delete queries;
  delete exact;
  delete edit;
  return EC_SUCCESS;
}

ErrorCode StartQuery(QueryID        query_id,
                     const char*    query_str,
                     MatchType      match_type,
                     unsigned int   match_dist) {
  auto q = new Query{
      query_id,
      true,
      match_type,
      match_dist,
      0,
  };
  auto unique_words = qs::hash_table<qs::string, entry>();
  char *q_str = strdup(query_str);
  qs::parse_string(q_str, " ", [q, &unique_words](qs::string &word) {
    q->word_count++;
    unique_words.insert(word, entry(word, nullptr));
  });

  switch (match_type) {
  case MT_EDIT_DIST:
    for (auto & unique_word : unique_words) {
      entry &en = *unique_word;
      auto found = edit->find(en);
      qs::vector<Query *> *qvec;
      if (found.is_empty()) {
        qvec = new qs::vector<Query *>();
        en.payload = qvec;
        edit->insert(en);
      } else {
        qvec = found.get().payload;
      }
      qvec->push(q);
    }
    break;
  case MT_HAMMING_DIST:
    for (auto & unique_word : unique_words) {
      entry &en = *unique_word;
      auto hamming_tree = hamming[en.word.length() - MIN_WORD_LENGTH];
      auto found = hamming_tree.find(en);
      qs::vector<Query *> *qvec;
      if (found.is_empty()) {
        qvec = new qs::vector<Query *>();
        en.payload = qvec;
        hamming_tree.insert(std::move(en));
      } else {
        qvec = found.get().payload;
      }
      qvec->push(q);
    }
    break;
  case MT_EXACT_MATCH:
    for (auto & unique_word : unique_words) {
      entry &en = *unique_word;
      auto f = exact->lookup(en.word);
      qs::vector<Query*>* qvec;
      if (f == exact->end()) {
        qvec = new qs::vector<Query *>();
        exact->insert(en.word, *qvec);
      } else {
        qvec = &(f->get());
      }
      qvec->push(q);
    }
    break;
  default:
    free(q_str);
    delete q;
    return EC_FAIL;
  }
  queries->insert(query_id, q);
  free(q_str);
  return EC_SUCCESS;
}

ErrorCode EndQuery(QueryID query_id) {
  auto i = queries->lookup(query_id);
  if (i == queries->end()) {
    return EC_FAIL;
  }
  (**i)->active = false;
  return EC_SUCCESS;
}

ErrorCode MatchDocument(DocID         doc_id,
                        const char*   doc_str) {
  for (auto & q : *queries) {
    auto query = *q;
    if (!query->active) continue;
    switch (query->match_type) {
    case MT_EXACT_MATCH:
    case MT_EDIT_DIST:
    case MT_HAMMING_DIST:
    default:
      return EC_FAIL;
    }
  }
  return EC_SUCCESS;
}

ErrorCode GetNextAvailRes(DocID*         p_doc_id,
                          unsigned int*  p_num_res,
                          QueryID**      p_query_ids) {
  return EC_SUCCESS;
}
