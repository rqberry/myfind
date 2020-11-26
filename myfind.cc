#include <fstream>      //needed for filesystem
#include <iostream>     //for cout
#include <vector>       //for vector
#include <filesystem>   //for the the thing that does our project for us lol
#include <algorithm>    //for std::find
#include <string>       //for compare()
#include <chrono>       //for -mtime (for chrono stuff)
#include <time.h>       //for tm struct and time_t
#include <dirent.h>     //for DIR
#include <iomanip>      //for something
#include <cstring>      //for strcpy()
#include <unistd.h>     //for execv()

namespace fs = std::filesystem;

/*
int execute(const char *pathname, std::vector<std::string> arg_list) {
  auto exec_iter = std::find(arg_list.begin(),arg_list.end(),"-exec");
  if (exec_iter != arg_list.end())
  {
    // creating argv on the stack
    size_t size_arglist = arg_list.size();
    char* argv[size_arglist];

    int i = 0;
    exec_iter++;
    char *arg;
    //TODO: print "find: missing argument to `-exec'" for no \; || ';'

    while (exec_iter < arg_list.end() &&
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
  }
}
*/

int main(int argc, char **argv) {

  std::vector<std::string> arg_list;
  for (int i = 1; i < argc; i++) arg_list.push_back((std::string)argv[i]);

  //for -name
  auto name_iter = std::find(arg_list.begin(),arg_list.end(),"-name");
  //TODO: print to error for no -name specification "find: missing argument to `-name'"
  std::string name_token = (name_iter != arg_list.end()) ? arg_list[std::distance(arg_list.begin(),name_iter)+1] : "";
  //TODO: print to error & exit gracefully
  if (name_token != "" && name_token.substr(0,1) == "/") {
      std::cout << "find: warning: ‘-name’ matches against basenames only, but the given pattern contains a directory separator (‘/’), thus the expression will evaluate to false all the time.  Did you mean ‘-wholename’?" << std::endl;
      return 1;
  }

  //for -mtime
  auto mtime_iter = std::find(arg_list.begin(),arg_list.end(),"-mtime");
  //TODO: print to error & exit gracefully
  if (++mtime_iter == arg_list.end())
  {
    std::cout << "find: missing argument to `-mtime'" << std::endl;
  }
  else if (arg_list[std::distance(arg_list.begin(),mtime_iter)+1].compare("0") != 0)
  {
    std::cout << "find: invalid argument `" << arg_list[std::distance(arg_list.begin(),mtime_iter)+1] << "' to `-mtime'" << std::endl;
  }
  bool mtime_token = (mtime_iter != arg_list.end()) ? true : false;

  //for -type
  //TODO: WHAT IF THERE ARE MULTIPLE '-type's !?!?!?!?
  //auto type_iter = std::find(arg_list.begin(),arg_list.end(),"-type");
  //char type_token = (type_iter != arg_list.end()) ? arg_list[type_iter+1] : (char) 0;

  //for -L
  fs::directory_options L_token = (std::find(arg_list.begin(),arg_list.end(),"-L") != arg_list.end()) ?
    fs::directory_options::follow_directory_symlink : fs::directory_options::none;

  //printing out the initial directory because recursive_directory_iterator always skips it
  //TODO: this is a hack
  if (name_token == "") {
      std::cout << "." << std::endl;
  }

  //recursive_directory_iterator iterates through all folders
  for (auto& p : fs::recursive_directory_iterator(".",L_token))
  {
    /*
    //testing to see if file_clock works
    //std::cout <<"the current time from file clock is "<<std::chrono::file_clock::now()<<std::endl();
    auto s = (std::string) p.path();

    std::string p_s = s.substr(1,s.length()-1);

    std::string name_token_comp = name_token;
    if (name_token == "") name_token_comp = *(--p.path().end());

    //std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    //std::time_t mtime_token_comp = now;
    //if (mtime_token) mtime_token_comp = std::chrono::system_clock::to_time_t(fs::last_write_time(p.path()));

    if (
      (*(--p.path().end())).compare(name_token_comp) == 0 //&&
      // mtime_token_comp >= now - 86400 &&
      // type qualification
      ) std::cout << "." << p_s << '\n';
    */
  }
  std::cout << "return 0" <<std::endl;
  return 0;
}
