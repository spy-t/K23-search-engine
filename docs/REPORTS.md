# Report 1

Το ζητούμενο API μετασχηματίστηκε έτσι ώστε να χρησιμοποιεί ένα πιο ιδιοματικό
C++ στυλ με RAII, templates, iterators και exceptions για error handling. Το mapping είναι
ως εξής:

- `entry -> include/qs/entry.hpp`

- `entry_list -> include/qs/list.hpp με template specialization το entry`

- `index -> include/qs/bk_tree.hpp με template specialization το entry`

- `enum error_code create_entry(const word* w, entry* e) -> entry constructor`

- `enum error_code destroy_entry(entry *e) -> entry destructor`

- `enum error_code create_entry_list(entry_list* el) -> list constructor`

- `unsigned int get_number_entries(const entry_list* el) -> list::get_size`

- `enum error_code add_entry(entry_list* el, const entry* e) -> list::append`

- `entry* get_first(const entry_list* el) -> list::begin`

- `entry* get_next(const entry_list* el) -> list::iterator::operator++()`

- `enum eror_code destroy_entry_list(entry_list* el) -> list destructor`

- `enum error_code build_entry_index(const entry_ist* el, enum match_type type, index* ix) -> bk_tree constructor`

- `enum error_code lookup_entry_index(const word* w, index* ix, int threshold, entry_list* result) -> bk_tree::match`

- `enum error_code destroy_entry_index(index* ix) -> bk_tree destructor`

Ακολουθούν κάποια design choices που πήραμε και κάποιες παραδοχές...

## Deduplication

Το deduplication υλοποιείται απλά με ένα hash set στο οποίο μπαίνει ότι
διαβάζεται με αποτέλεμσα να υπάρχουν όλες οι λέξεις το πολύ μια φορά όταν βγουν.

## Entry list

Το entry list είναι ένα απλό doubly linked list

## BK tree

Τα κάθε node του bk tree κρατάει τα παιδιά του σε ένα skip list έτσι ώστε να
είναι ταξινομημένα γιατί TODO(yannis): εξήγησε γιατί

## Testing

Για τα τεστ χρησιμοποιήσαμε το catch2 το οποίο μας ανάγκασε στο `query_test` να
χρησιμοποιήσουμε το `std::string` του STL. Επειδή είναι σε τεστ κώδικα δεν
θεωρήσαμε οτι υπάρχει πρόβλημα με αυτό.

### End to end test

Το `query_test` είναι ένα τεστ το οποίο εξετάζει ολιστικά τα ζητούμενα της
εργασίας. Επειδή χρειάζεται command line options για να λειτουργήσει όταν τρέχει
όλη η σουίτα με το `meson test` το `query_test` απλά περνάει αυτόματα για να μην
επηρεάσει τα υπόλοιπα test.

Το `query_test` χρειάζεται μια προεργασία για να τρέξει. Αρχικά πρέπει να
υπάρχει κάποιο input το οποίο θα χρησιμοποιήσει το test. Για αρχή κάνοντας `cd src/test/resources`
τρέχουμε `tar xvzf data_set.tar.gz` το οποίο έχει 2 αρχεία το `query_list` και
το `word_list`. Με την προϋπόθεση οτι υπάρχει `python3` τρέχουμε 
`python3 process.py word_list` και `python3 process.py query_list` αντίστοιχα
για να παράξουμε αρχεία τα οποία έχουν λέξεις ίδιου μήκους για ελέγχους με το 
hamming distance.

Έχωντας τα προαπαιτούμενα μετά μπορούμε απο το `build` directory να τρέξουμε πχ
`./query_test --queries path/to/query_list --words path/to/word_list --threshold 3 --distance edit`

**ΠΡΟΣΟΧΗ**: αν δεν μπει το `threshold` flag τότε το τεστ θα κάνει pass
κατευθείαν.

Παράδειγμα με hamming distance

`./query_test --queries path/to/3-query_list --words path/to/3-word_list --threshold 3 --distance hamming`
