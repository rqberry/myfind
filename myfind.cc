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

namespace fs = std::filesystem;

std::string name_token = "";
double mtime_token = -1;
std::string type_token = "";

//this is strtok_r but for c++
// i included the _r because this version of strtok isn't hellish
// i dont think this uses the same algoritm as _r through

std::vector<std::string> finalize_tokens(std::vector<std::string> &args){
  std::vector<std::string> finalized_tokens;
  size_t i = 0;
  while(i < args.size()) {
    std::string current_word = args[i];
    if (current_word[0] == '"' ){
      i++;
      while(args[i][args[i].length()-1] != '\"' && i < args.size())
      {
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

//to parse, we first separate by spaces. then we go through that vector, and merge all the things in quotes
int parse_name(std::vector<std::string> args) {
  if (args.empty())
  {
    std::cerr<<"find: missing argument to `-name\'"<<std::endl;
    return 1;
  }

  //create a list of all the tokens

  //go through the tokens, anything inside quotes is one argument
  args = finalize_tokens(args);

  name_token = args.front();
  args.erase(args.begin());
  if (!args.empty())
  {
    std::cerr<<"find: paths must precede expression: `"<<args.front()<<"\'"<<std::endl;
    return 1;
  }
  return 0;
  //TODO: for / at end or maybe anywhere?
  if (!name_token.empty() && name_token[0] == '/') {
    std::cerr << "find: warning: ‘-name’ matches against basenames only, but the given pattern contains a directory separator (‘/’), thus the expression will evaluate to false all the time.  Did you mean ‘-wholename’?" << std::endl;
    return 1;
  }

  return 0;
}

int parse_mtime(std::vector<std::string> args) {
  if (args.empty())
  {
    std::cerr<<"find: missing argument to `-mtime\'"<<std::endl;
    return 1;
  }
  args = finalize_tokens(args);

  char* endptr = 0;
  mtime_token = strtod(args.front().c_str(), &endptr);
  if(*endptr != '\0' || endptr == args.front().c_str())
  {
    std::cerr << "find: invalid argument `" << args.front() << "' to `-mtime'" << std::endl;
    return 1;
  }
  args.erase(args.begin());
  if (!args.empty())
  {
    std::cerr<<"find: paths must precede expression: `"<<args.front()<<"\'"<<std::endl;
    return 1;
  }
  return 0;
}


//this should return a list of the types we're looking for
int parse_type(std::vector<std::string> args) {
  //first parse the args, delimiting with commas
  if (args.empty())
  {
    std::cerr<<"find: missing argument to `-type\'"<<std::endl;
    return 1;
  }

  //go through the tokens, anything inside quotes is one argument
  args = finalize_tokens(args);

  type_token = args.front();
  args.erase(args.begin());

  if (type_token[type_token.size()] == ',')
  {
    std::cout << "find: Last file type in list argument to -type is missing, i.e., list is ending on: ','" << '\n';
    return 1;
  }

  //remove ',' delimiters
  type_token.erase(std::remove(type_token.begin(),type_token.end(),','),type_token.end());

  //if type_token contains a space, print an error to cerr
  std::string chars = "bcdpfls";
  for (auto s : type_token){
    if (s == ' ')
    {
      std::cerr<<"find: Must separate multiple arguments to -type using: ','"<<std::endl;
      return 1;
    }
    if (chars.find(s) == std::string::npos) {
      std::cerr << "find: Unknown argument to -type: " << s << std::endl;
      return 1;
    }
  }
  if (!args.empty())
  {
    std::cerr<<"find: paths must precede expression: `"<<args.front()<<"\'"<<std::endl;
    return 1;
  }

  return 0;
}


//todo: make sure it checks path, not just local files
int parse_exec(std::vector<std::string> args) {
  /*
  if (args.empty())
  {
    std::cerr<<"find: missing argument to `-exec\'"<<std::endl;
    return 1;
  }
  // creating argv on the stack
  char* char_args[args.size()];

  int i = 0;
  char *arg;
  //char * cstr = new char [str.length()+1];
  ///std::strcpy (cstr, str.c_str());
  // DO NOT FORGET TO DELETE[] cstr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


  //TODO: print "find: missing argument to `-exec'" for no \; || ';'
  while (i < args.size() &&
    (arg_list[std::distance(arg_list.begin(),exec_iter)].compare("\\;") != 0 ||
    arg_list[std::distance(arg_list.begin(),exec_iter)].compare("';'") != 0))
  {
    strcpy(arg, arg_list[std::distance(arg_list.begin(),exec_iter)].c_str());
    argv[i] = arg;
    i++;
  }

  argv[i] = nullptr;
  execv(pathname, const_cast<char**>(argv));
  //TODO: also remember to assert if the exec returns not 0;
  //TODO: execv errors must be find: errors (we can just append find: to begin of the execv error)
  return 0;
  */
}


// loops through arg_list, parses each argument.
int arg_interpret(std::vector<std::string> &arg_list) {
  std::string cmd = arg_list[0];
  arg_list.erase(arg_list.begin());

  if (cmd.compare("-name") == 0) {
    if (parse_name(arg_list)) return 1;
  } else if (cmd.compare("-mtime") == 0) {
    if (parse_mtime(arg_list)) return 1;
  } else if (cmd.compare("-type") == 0) {
    if (parse_type(arg_list)) return 1;
  } else if (cmd.compare("-exec") == 0) {
    if (parse_exec(arg_list)) return 1;
  } else {
    std::cerr<<"find: unknown predicate `"<<cmd<<"\'"<<std::endl;
    return 1;
  }
  return 0;
}

bool type_match(char type_token, fs::path p, fs::directory_options L_token) {
  if (type_token == 'b') return fs::is_block_file(fs::status(p));
  if (type_token == 'c') return fs::is_character_file(fs::status(p));
  if (type_token == 'd') return fs::is_directory(fs::status(p));
  if (type_token == 'p') return fs::is_fifo(fs::status(p));
  if (type_token == 'f') return fs::is_regular_file(fs::status(p));
  if (type_token == 'l' && L_token != fs::directory_options::follow_directory_symlink) return fs::is_symlink(fs::symlink_status(p));   //need the link not the target
  if (type_token == 's') return fs::is_socket(fs::status(p));
  return false;
}


int print_entry(fs::path p, fs::directory_options L_token/*, std::string p_print, , fs::path start_path*/) {
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  //this is for print

  //std::string proximate_path = fs::proximate(p,start_path).string();
  //for -name
  std::string name_token_comp = name_token;
  if (name_token == "") name_token_comp = *(--p.end());

  //for -mtime
  std::time_t mtime_token_comp = now;
  if (mtime_token != -1) mtime_token_comp = std::chrono::system_clock::to_time_t(fs::last_write_time(p));

  //for -type
  bool type_token_comp;

  if (!type_token.empty()) {
    for(char c : type_token) {
      type_token_comp |= type_match(c,p,L_token);
    }
  } else {
     type_token_comp = true;
  }

  //The Holy Mr. If Statment
  if (
    (*(--p.end())).compare(name_token_comp) == 0 &&
    mtime_token_comp >= (now - (mtime_token+1)*86400) &&
    mtime_token_comp <= (now - (mtime_token)*86400) &&
    type_token_comp
  ) std::cout <<p.string()<< '\n';
  return 0;
}

int main(int argc, char **argv) {

  //turning argv into a vector of strings because #yolo
  std::vector<std::vector<std::string>> arg_list;

  int i = 1;
  fs::directory_options L_token = fs::directory_options::none;
  if (argc > 1 && std::string(argv[i]).compare("-L") == 0) {
    L_token = fs::directory_options::follow_directory_symlink;
    i++;
  }

  //TODO: std::cerr<<"find: unknown predicate `-L'"<<std::endl;
  while (i < argc) {
    //if the current argument in argv starts with a -, we include all the args for that arg after it, until the next arg that starts with a hyphen
    std::vector<std::string> arg;
    arg.push_back(std::string(argv[i]));
    if (argv[i][0] == '-') {
      i++;
      while (i < argc && argv[i][0] != '-'){
        arg.push_back(std::string(argv[i]));
        i++;
      }
    } else {
      i++;
    }
    arg_list.push_back(arg);
  }

  //for start_path
  fs::path start_path = fs::current_path();
  fs::path search_path = ".";
  if (!arg_list.empty() && arg_list[0][0][0] != '-')
  {
    search_path = arg_list[0][0];
    if (!fs::exists(search_path)) {
      std::cout << "find: ‘"<<search_path.string()<<"’: No such file or directory" << '\n';
      return 1;
    }
    arg_list.erase(arg_list.begin());
  }

  //arg_interpret each arg
  while(!arg_list.empty()) {

    if(arg_interpret(arg_list[0]) == 1) {
      return 1;
    }
    arg_list.erase(arg_list.begin());
  }

  if (!(L_token == fs::directory_options::follow_directory_symlink && !fs::is_symlink(fs::symlink_status(search_path))))
  {
    if(print_entry(search_path,L_token)) return 1;
  }
  for (auto& p : fs::recursive_directory_iterator(search_path,L_token))
  {
    if(print_entry(p.path(),L_token)) return 1;
  }
  return 0;
}
