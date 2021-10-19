#ifndef PLAINLIB_PROMPT_HPP
#define PLAINLIB_PROMPT_HPP

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "hash_table.hpp"
#include "vector.hpp"

namespace pl {
class prompt;

using command_function_t = typename std::add_pointer<void(
    std::size_t argc, const vector<std::string> &argv)>::type;

class prompt_exception : public std::exception {
  const char *reason;

public:
  prompt_exception(const char *reason) : reason(reason) {}
  prompt_exception(std::string reason) : reason(reason.c_str()) {}

  const char *what() const throw() { return reason; }
};

class prompt_command {
public:
  prompt_command(std::string name, command_function_t fn)
      : name(std::move(name)), command_function(fn){};

  const std::string name;
  command_function_t command_function;
};

class prompt {
  pl::hash_table<prompt_command> p_commands;
  std::string prompt_symbol;

  void execute(const std::string &cmd, const vector<std::string> &args) {
    auto cmdi = p_commands.lookup(cmd);
    if (cmdi != nullptr) {
      auto cmd = cmdi->get();
      cmd.command_function(args.get_size(), args);
    } else {
      std::cout << "Command " << cmd << " not found\n";
    }
  }

public:
  prompt(std::string prompt_symbol) : prompt_symbol(prompt_symbol){};

  void register_command(prompt_command cmd) {
    p_commands.insert(cmd.name, cmd);
  }

  void start() {
    std::cout << prompt_symbol;
    for (std::string line; std::getline(std::cin, line);) {

      std::stringstream line_stream(line);
      vector<std::string> args;
      std::string cmd;
      std::size_t i = 0;
      for (std::string token; std::getline(line_stream, token, ' ');) {
        if (i == 0) {
          cmd = token;
        } else {
          args.push(token);
        }
        ++i;
      }
      if (!cmd.empty()) {
        if (cmd == "exit") {
          return;
        }

        try {
          execute(cmd, args);
        } catch (const prompt_exception &e) {
          std::cout << e.what() << "\n";
        } catch (const std::exception &e) {
          throw;
        }
      }
      std::cout << "\n";
      std::cout << prompt_symbol;
    }
  }

  // This will block untile there is input on stdin so if you want asynchronous
  // behaviour call it when you know there is data on stdin for example after a
  // call to poll. Returns true if the command was "exit"
  bool poll() {
    std::string line;
    std::getline(std::cin, line);
    std::stringstream line_stream(line);
    vector<std::string> args;
    std::string cmd;
    std::size_t i = 0;
    for (std::string token; std::getline(line_stream, token, ' ');) {
      if (i == 0) {
        cmd = token;
      } else {
        args.push(token);
      }
      ++i;
    }
    if (!cmd.empty()) {
      if (cmd == "exit") {
        return true;
      }

      try {
        execute(cmd, args);
      } catch (const prompt_exception &e) {
        std::cout << e.what() << "\n";
      } catch (const std::exception &e) {
        throw;
      }
    }
    std::cout << "\n";
    std::cout << prompt_symbol;
    std::cout.flush();
    return false;
  }
};
}; // namespace pl
#endif
