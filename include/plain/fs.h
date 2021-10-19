#ifndef PLAINLIB_FS_H
#define PLAINLIB_FS_H

#include <cstdint>
#include <plain/vector.hpp>
#include <string>

namespace pl {
const pl::vector<std::string> ls(const std::string &path);
const pl::vector<std::string> ls(const char *path, std::size_t len);
std::string fd_to_filename(int fd);
std::string get_basename(const std::string &path);
// Extracts every member of a unix path
pl::vector<std::string> get_path_members(const std::string &path);
} // namespace pl
#endif
