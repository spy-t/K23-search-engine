# Report 1

Το ζητούμενο API μετασχηματίστηκε έτσι ώστε να χρησιμοποιεί ένα πιο ιδιοματικό
C++ στυλ με RAII templates και exceptions για error handling. Το mapping είναι
ως εξής:

entry -> src/bin/entry.hpp
entry_list -> include/qs/list.hpp με template specialization το entry
index -> include/qs/bk_tree.hpp με template specialization το entry

enum error_code create_entry(const word* w, entry* e) -> entry constructor
enum error_code destroy_entry(entry *e) -> entry destructor
enum error_code create_entry_list(entry_list* el) -> list constructor
unsigned int get_number_entries(const entry_list* el) -> list::get_size
enum error_code add_entry(entry_list* el, const entry* e) -> list::append
entry* get_first(const entry_list* el) -> list::begin
entry* get_next(const entry_list* el) -> list::iterator::operator++()
enum eror_code destroy_entry_list(entry_list* el) -> list destructor
enum error_code build_entry_index(const entry_ist* el, enum match_type type, index* ix) -> bk_tree constructor
enum error_code lookup_entry_index(const word* w, index* ix, int threshold, entry_list* result) -> bk_tree::match
enum error_code destroy_entry_index(index* ix) -> bk_tree destructor

Ακολουθούν κάποια design choices που πήραμε και κάποιες παραδοχές...
