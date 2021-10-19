#ifndef PLAINLIB_BLOOM_H
#define PLAINLIB_BLOOM_H
#include <cstdint>
#include <cstdlib>
#include <string>

namespace pl {

class bloom {

  uint8_t *bit_field;
  size_t size;

  size_t hash_functions;

public:
  bloom() = delete;

  bloom(size_t size, size_t num_of_elements);
  bloom(size_t size, uint8_t *bit_field, size_t hash_functions);

  ~bloom();

  bloom(const bloom &other);
  bloom &operator=(const bloom &other);

  void add(const uint8_t *el);
  void add(const std::string &el);

  bool lookup(const uint8_t *el);
  bool lookup(const std::string &el);

  size_t get_hash_functions() const { return hash_functions; }
  size_t get_size() const { return size; }
  const uint8_t *get_bit_field() const { return bit_field; }

  void merge(std::size_t size, uint8_t *bytes);
};

} // namespace pl
#endif