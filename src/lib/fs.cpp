#include <cstring>
#include <dirent.h>
#include <exception>
#include <plain/fs.h>
#include <plain/vector.hpp>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace qs {
const qs::vector<std::string> ls(const std::string &path) {
  return ls(path.c_str(), path.length());
}

const qs::vector<std::string> ls(const char *path, std::size_t len) {
  qs::vector<std::string> vec;
  const char *sep = path[len - 1] == '/' ? "" : "/";
  DIR *d = opendir(path);
  if (d == nullptr) {
    throw std::runtime_error("Could not open dir " + std::string(path));
  }
  try {
    struct dirent *res = nullptr;
    while ((res = readdir(d)) != nullptr) {
      if (strcmp(res->d_name, ".") == 0 || strcmp(res->d_name, "..") == 0) {
        // Ignore dot entries
        continue;
      }
      std::string entry_path;
      std::stringstream ss(entry_path);
      ss << path << sep << res->d_name;
      vec.push(ss.str());
    }
  } catch (std::exception &e) {
    closedir(d);
    throw;
  }
  closedir(d);
  return vec;
}

std::string fd_to_filename(int fd) {
  struct stat sb;
  std::stringstream fdpath;
  fdpath << "/proc/self/fd/" << fd;
  std::string proc_path = fdpath.str();
  if (lstat(proc_path.c_str(), &sb) == -1) {
    return std::string();
  }
  char *link = new char[sb.st_size + 1];
  int ret = readlink(proc_path.c_str(), link, sb.st_size + 1);
  if (ret == -1) {
    return std::string();
  }

  link[sb.st_size] = '\0';

  std::string str(link);
  delete[] link;

  return str;
}

std::string get_basename(const std::string &path) {
  qs::vector<std::string> tokens;
  std::istringstream istream(path);

  for (std::string token; std::getline(istream, token, '/');) {
    tokens.push(token);
  }
  auto size = tokens.get_size();
  if (tokens[size - 1] == "") {
    return tokens[size - 2];
  } else {
    return tokens[size - 1];
  }
}

qs::vector<std::string> get_path_members(const std::string &path) {
  std::stringstream ss(path);
  qs::vector<std::string> path_members;
  for (std::string token; std::getline(ss, token, '/');) {
    path_members.push(token);
  }

  return path_members;
}
} // namespace qs
