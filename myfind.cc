#include <fstream>      //needed for filesystem
#include <iostream>     //for cout
#include <vector>       //for vector
#include <filesystem>   //for the the thing that does our project for us lol
#include <algorithm>    //for std::find
#include <string>       //for compare() and string and str.find
#include <chrono>       //for -mtime (for chrono stuff)
#include <time.h>       //for tm struct and time_t
#include <dirent.h>     //for DIR
#include <iomanip>      //for something
#include <cstring>      //for strcpy()
#include <unistd.h>     //for execv()
#include <string.h>     //for strcmp

namespace fs = std::filesystem;


//this takes a string of vectors and parses the arguments to a format that's easily parseable
//it merges things inside quotes.
std::vector<std::string> finalize_tokens(std::vector<std::string> &args) {
  std::vector<std::string> finalized_tokens;
  size_t i = 0;
  while (i < args.size()) {
    std::string current_word = args[i];
    if (current_word[0] == '"' ) {
      i++;
      while (args[i][args[i].length()-1] != '\"' && i < args.size()) {
        current_word.append(args[i]);
        i++;
      }
    } else {
      i++;
    }
    finalized_tokens.push_back(current_word);
  }
  return finalized_tokens;
}

//this is a helper function for the -name argument. it just checks to make sure that -name is valid.
std::string parse_name(std::vector<std::string> args) {
  if (args.empty()) {
    std::cerr<<"find: missing argument to `-name\'"<<std::endl;
    return "";
  }
  //pop the first argument to -name
  std::string name_token = args.front();
  args.erase(args.begin());
  //if there's more than one argument to -name, throw an error
  if (!args.empty()) {
    std::cerr<<"find: paths must precede expression: `"<<args.front()<<"\'"<<std::endl;
    if (fs::exists(args.front())) std::cerr<<"find: possible unquoted pattern after predicate `-name'?"<<std::endl;
    return "";
  }
  //for / anywhere in the argument
  if (name_token.find('/') != std::string::npos) {
    std::cerr << "find: warning: ‘-name’ matches against basenames only, but" <<
    " the given pattern contains a directory separator (‘/’), thus the expres"<<
    "sion will evaluate to false all the time.  Did you mean ‘-wholename’?"
    <<std::endl;
    return "";
  }
  return name_token;
}


//this is a helper function for -mtime. it makes sure the argument for -mtime is valid
double parse_mtime(std::vector<std::string> args) {
  if (args.empty()) {
    std::cerr<<"find: missing argument to `-mtime\'"<<std::endl;
    return -1;
  }
  //here we check to make sure -mtime only gets good arguments
  char* endptr = 0;
  double mtime_token = strtod(args.front().c_str(), &endptr);
  if (*endptr != '\0' || endptr == args.front().c_str()) {
    std::cerr << "find: invalid argument `" << args.front() << "' to `-mtime'"
    << std::endl;
    return -1;
  }
  //check that mtime doesn't have too many arguments
  args.erase(args.begin());
  if (!args.empty()) {
    std::cerr<<"find: paths must precede expression: `"<<args.front()<<"\'"<<std::endl;
    if (fs::exists(args.front())) std::cerr<<"find: possible unquoted pattern after predicate `-mtime'?"<<std::endl;
    return -1;
  }
  return mtime_token;
}


//this should return a list of the types we're looking for
std::string parse_type(std::vector<std::string> args) {
  if (args.empty()) {
    std::cerr<<"find: missing argument to `-type\'"<<std::endl;
    return "";
  }

  //popping the first argument
  std::string type_token = args.front();
  args.erase(args.begin());
  //these errors should be super self explanatory. it's pretty obvious what we're doing here
  if (type_token[type_token.size()-1] == ',') {
    std::cerr << "find: Last file type in list argument to -type is missing,"<<
    " i.e., list is ending on: ','" << std::endl;
    return "";
  }

  //checking to make sure type_token is a comma separated list
  std::string chars = "bcdpfls";
  bool not_delim = true;
  for (auto s : type_token) {
    //it goes back and forth between expecting a comma (not_delim = false) and expecting one of chars
    if (not_delim) {
      if (chars.find(s) == std::string::npos) {
        std::cerr << "find: Unknown argument to -type: " << s << std::endl;
        return "";
      }
      not_delim = false;
    } else {
      if (s != ',') {
        std::cerr<<"find: Must separate multiple arguments to -type using: ','"
        <<std::endl;
        return "";
      }
      not_delim = true;
    }
  }

  if (!args.empty()) {
    std::cerr<<"find: paths must precede expression: `"<<args.front()<<"\'"<<std::endl;
    if (fs::exists(args.front())) std::cerr<<"find: possible unquoted pattern after predicate `-type'?"<<std::endl;
    return "";
  }

  //remove delimiters
  type_token.erase(std::remove(type_token.begin(),type_token.end(),','),type_token.end());

  return type_token;
}

int parse_exec(std::vector<std::string> args) {
  if (args.empty()) {
    std::cerr<<"find: missing argument to `-exec\'"<<std::endl;
    return 1;
  }

  if (args.back().compare(";") != 0) {
    std::cerr << "find: missing argument to `-exec'" << std::endl;
    return 1;
  } else {
    args.pop_back();
  }
  return 0;
}

int parse_print(std::vector<std::string> args) {
  if (!args.empty()) {
    std::cerr<<"find: paths must precede expression: `"<<args.front()<<"\'"<<std::endl;
    if (fs::exists(args.front())) std::cerr<<"find: possible unquoted pattern after predicate `-print'?"<<std::endl;
    return 1;
  }
  return 0;
}

bool type_match(char type_token, fs::path p, fs::directory_options L_token) {
  if (type_token == 'b') return fs::is_block_file(fs::status(p));
  if (type_token == 'c') return fs::is_character_file(fs::status(p));
  if (type_token == 'd') return fs::is_directory(fs::status(p)) && !fs::is_symlink(fs::symlink_status(p));// && L_token == fs::directory_options::none));
  if (type_token == 'p') return fs::is_fifo(fs::status(p));
  if (type_token == 'f') return fs::is_regular_file(fs::status(p));
  if (type_token == 'l' && L_token == fs::directory_options::none) return fs::is_symlink(fs::symlink_status(p))/* && !fs::is_directory(fs::status(p))*/;   //need the link not the target
  if (type_token == 's') return fs::is_socket(fs::status(p));
  return false;
}


int print_entry(fs::path p,
                fs::directory_options L_token,
                std::vector<std::vector<std::string>> exec_args,
                std::vector<std::string> name_tokens,
                std::vector<double> mtime_tokens,
                std::vector<std::string> type_tokens,
                bool print_token) {

  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  //this is for print

  //for -name
  bool name_bool = true;
  for (std::string name_token : name_tokens) {
    if (name_token.compare(*(--p.end())) != 0) name_bool = false;
  }

  //for -mtime
  bool mtime_bool = true;
  for (double mtime_token : mtime_tokens) {
    time_t p_write = std::chrono::system_clock::to_time_t(fs::last_write_time(p));
    if (!(p_write >= (now - (mtime_token+1)*86400) &&
      p_write <= (now - (mtime_token)*86400))) {
        mtime_bool = false;
    }
  }

  //for -type
  bool type_bool = true;
  if (!type_tokens.empty()) {
    for (std::string type_token : type_tokens) {
      bool token_bool = false;
      for (char c : type_token) {
        if (token_bool || type_match(c,p,L_token)) {
          token_bool = true;
        } else {
          token_bool = false;
        }
      }
      if (type_bool && token_bool) {
        type_bool = true;
      } else {
        type_bool = false;
      }
    }
  }

  //The Holy Mr. If Statment
  if (name_bool && mtime_bool && type_bool) {
    if (!exec_args.empty()) {
      //for -exec
      std::string exec_arg;
      for (std::vector<std::string> args : exec_args) {
        for (std::string arg : args) {
          size_t start = arg.find("{}");
          if (start != std::string::npos) arg.replace(start,2,p.string());
          exec_arg += arg;
          if (arg.compare(args.back()) != 0) exec_arg += " ";
        }
        if (system(exec_arg.c_str()) == 0 && print_token) std::cout << p.string()<< std::endl;
      }
    } else std::cout << p.string() << std::endl;
  }
  return 0;
}

int main(int argc, char **argv) {

  //turning argv into a vector of strings
  std::vector<std::vector<std::string>> arg_list;

  int i = 1;
  fs::directory_options L_token = fs::directory_options::none;
  if (argc > 1 && std::string(argv[i]).compare("-L") == 0) {
    L_token = fs::directory_options::follow_directory_symlink;
    i++;
  }

  while (i < argc) {
    //if the current argument in argv starts with a -, we include all the args for that arg after it, until the next arg that starts with a hyphen
    std::vector<std::string> arg;
    arg.push_back(std::string(argv[i]));
    if (argv[i][0] == '-') {
      i++;
      while (i < argc && argv[i][0] != '-') {
        arg.push_back(std::string(argv[i]));
        i++;
      }
      // merge any quoted sections of arg
      finalize_tokens(arg);
    } else {
      i++;
    }
    arg_list.push_back(arg);
  }

  //for start_path
  fs::path start_path = fs::current_path();
  std::vector<fs::path> search_paths;
  bool push_dot = true;

  while (!arg_list.empty() && arg_list[0][0][0] != '-') {
    search_paths.push_back(arg_list[0][0]);
    if (!fs::exists(search_paths.back())) {
      std::cerr << "find: ‘"<<search_paths.back().string()<<"’: No such file or directory" << std::endl;
      search_paths.pop_back();
      push_dot = false;
    }
    arg_list.erase(arg_list.begin());
  }

  if (push_dot && search_paths.empty()) search_paths.push_back(".");

  std::vector<std::string> name_tokens;
  std::vector<double> mtime_tokens;
  std::vector<std::string> type_tokens;
  bool print_token = false;

  //arg_interpret each arg
  std::vector<std::vector<std::string>> exec_args;
  while (!arg_list.empty()) {

    std::string cmd = arg_list[0][0];
    arg_list[0].erase(arg_list[0].begin());

    if (cmd.compare("-name") == 0) {
      name_tokens.push_back(parse_name(arg_list[0]));
      if (name_tokens.back().empty()) return 1;
    } else if (cmd.compare("-mtime") == 0) {
      mtime_tokens.push_back(parse_mtime(arg_list[0]));
      if (mtime_tokens.back() == -1) return 1;
    } else if (cmd.compare("-type") == 0) {
      type_tokens.push_back(parse_type(arg_list[0]));
      if (type_tokens.back().empty()) return 1;
    } else if (cmd.compare("-exec") == 0) {
      //this just reformats the args for -exec
      if (parse_exec(arg_list[0])) return 1;
      exec_args.push_back(arg_list[0]);
    } else if (cmd.compare("-print") == 0) {
      if (parse_print(arg_list[0]) != 0) return 1;
      print_token = true;
    } else {
      std::cerr<<"find: unknown predicate `"<<cmd<<"\'"<<std::endl;
      return 1;
    }

    arg_list.erase(arg_list.begin());
  }

  for (fs::path search_path : search_paths) {
    //find search_path
    if (print_entry(search_path,
                    L_token,
                    exec_args,
                    name_tokens,
                    mtime_tokens,
                    type_tokens,
                    print_token))
      return 1;
    //find subdirectories
    if (fs::is_directory(fs::status(search_path)) ||
       (fs::is_symlink(fs::symlink_status(search_path)) &&
       L_token == fs::directory_options::none)) {
      for (auto& p : fs::recursive_directory_iterator(search_path,L_token)) {
        //print entries runs all the commands
        if (print_entry(p.path(),
                        L_token,
                        exec_args,
                        name_tokens,
                        mtime_tokens,
                        type_tokens,
                        print_token))
          return 1;
      }
    }
  }
  return 0;
}
