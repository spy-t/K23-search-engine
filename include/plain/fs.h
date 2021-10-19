#ifndef QS_FS_H
#define QS_FS_H

#include <cstdint>
#include <plain/vector.hpp>
#include <string>

namespace qs {
const qs::vector<std::string> ls(const std::string &path);
const qs::vector<std::string> ls(const char *path, std::size_t len);
std::string fd_to_filename(int fd);
std::string get_basename(const std::string &path);
// Extracts every member of a unix path
qs::vector<std::string> get_path_members(const std::string &path);
} // namespace qs
#endif
