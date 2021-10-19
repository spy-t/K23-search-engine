#include <cmath>

#include <cstring>
#include <plain/bloom.h>
#include <plain/hash.h>
#include <stdexcept>

namespace qs {

static void bf_set_bit(uint8_t *bf, size_t pos, std::size_t bf_size);
static uint8_t bf_get_bit(const uint8_t *bf, std::size_t pos,
                          std::size_t bf_size);
static size_t calculate_k(std::size_t size, std::size_t number_of_elements);

bloom::bloom(std::size_t size, std::size_t num_of_elements)
    : bit_field(new uint8_t[size]), size(size),
      hash_functions(calculate_k(size, num_of_elements)) {}

bloom::bloom(size_t size, uint8_t *bit_field, size_t hash_functions)
    : bit_field(bit_field), size(size), hash_functions(hash_functions) {}

bloom::bloom(const bloom &other)
    : bit_field(new uint8_t[other.size]), size(other.size),
      hash_functions(other.hash_functions) {
  std::memcpy(bit_field, other.bit_field, other.size);
};

bloom &bloom::operator=(const bloom &other) {
  if (this != &other) {
    size = other.size;
    hash_functions = other.hash_functions;
    bit_field = new uint8_t[other.size];
    std::memcpy(bit_field, other.bit_field, other.size);
  }
  return *this;
}

bloom::~bloom() { delete[] bit_field; }

void bloom::add(const uint8_t *el) {
  for (size_t i = 1; i <= hash_functions; i++) {
    uint64_t pos = hash_i(el, i);
    bf_set_bit(bit_field, pos, size);
  }
}
void bloom::add(const std::string &el) { add((uint8_t *)el.c_str()); }

bool bloom::lookup(const uint8_t *el) {
  for (size_t i = 1; i <= hash_functions; i++) {
    uint64_t pos = hash_i(el, i);
    if (bf_get_bit(bit_field, pos, size) == 0) {
      return false;
    }
  }
  return true;
}

bool bloom::lookup(const std::string &el) {
  return lookup((uint8_t *)el.c_str());
}

void bloom::merge(std::size_t size, uint8_t *bytes) {
  if (this->size != size) {
    // Can't merge bloom filters of different sizes
    throw std::runtime_error("can't merge bloom filters of different sizes");
  }

  for (std::size_t i = 0; i < this->size; ++i) {
    // OR the bytes
    bit_field[i] |= bytes[i];
  }
}

void bf_set_bit(uint8_t *bf, size_t pos, std::size_t bf_size) {
  // Constrain the array position within the bit field bounds
  pos = pos % (bf_size * 8);

  // Shift a bit to the correct position within a byte
  uint8_t bit = 1 << pos % 8;

  // OR it to set it
  bf[pos / 8] |= bit;
}

uint8_t bf_get_bit(const uint8_t *bf, size_t pos, std::size_t bf_size) {
  // Constrain the array position within the bit field bounds
  pos = pos % (bf_size * 8);

  // Shift a bit to the correct position within a byte
  uint8_t bit = 1 << pos % 8;

  return bf[pos / 8] & bit;
}

static size_t calculate_k(size_t size, size_t number_of_elements) {
  if (number_of_elements == 0) {
    // Arbitrary number to get the bloom filter going.
    number_of_elements = 1000;
  }
  double div = (size * 8) / number_of_elements;
  double ln2 = log(2);

  auto k = (size_t)round(div * ln2);
  return k == 0 ? 1 : k;
}
} // namespace qs
