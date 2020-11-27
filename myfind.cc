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

/*
void main_loop_r(fs::path top_p, fs::directory_options L_token, std::string name_token, bool mtime_token) {

  (void) top_p;
  const std::string ass =  ".";
  fs::directory_iterator dir_iterator(ass,L_token);
  for(auto& p : dir_iterator)
  {
    //this is for print
    std::string dir_entry_printable = ((std::string)p.path()).substr(1,((std::string)p.path()).length()-1);

    //for -name
    std::string name_token_comp = name_token;
    if (name_token == "") name_token_comp = *(--p.path().end());

    //for -mtime
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::time_t mtime_token_comp = now;
    if (mtime_token) mtime_token_comp = std::chrono::system_clock::to_time_t(fs::last_write_time(p.path()));

    //The Holy Mr. If Statment
    if (
      (*(--p.path().end())).compare(name_token_comp) == 0 &&
      mtime_token_comp >= now - 86400 // &&
      // type qualification
    ) std::cout << "." << dir_entry_printable << '\n';

    //if we need to recur
    if (p.is_directory())
      main_loop_r(p, L_token, name_token, mtime_token);
    }

}
*/

int main(int argc, char **argv) {

  //turning argv into a vector of strings because #yolo
  std::vector<std::string> arg_list;
  for (int i = 1; i < argc; i++) arg_list.push_back((std::string)argv[i]);

  //for -name, we find name if it exists and then get the argument "name_token" for future usage
  auto name_iter = std::find(arg_list.begin(),arg_list.end(),"-name");

  std::string name_token = "";

  //getting name token, this is pretty self explanatory. if the name iter points to the end, there is no name_token for us to get
  // otherwise, we get whatever's after... idk if this works????????? shouldn't it be checking whatever's after name_iter?
  // i think this has a bug, TODO: check if the following is right
  try{
    name_token = (name_iter != arg_list.end()) ? arg_list[std::distance(arg_list.begin(),name_iter)+1] : "";
  }
  catch (int e){
    //TODO: print to error for no -name specification "find: missing argument to `-name'"

    //if the above fails, it means there's an array out of bounds error where arglist don't have arglist[stuff+1]
    //aka, there is no argument for -name!!!
    std::cout<<"find: missing argument to `-name'"<<std::endl;
    return 1;
  }

  //TODO: print to error & exit gracefully
  //TODO: find: warning: ‘-name’ matches against basenames only, but the given pattern contains a directory separator (‘/’), thus the expression will evaluate to false all the time.  Did you mean ‘-wholename’?
  //  for / at end or maybe anywhere?
  if (name_token != "" && name_token.substr(0,1) == "/") {
      std::cout << "find: warning: ‘-name’ matches against basenames only, but the given pattern contains a directory separator (‘/’), thus the expression will evaluate to false all the time.  Did you mean ‘-wholename’?" << std::endl;
      return 1;
  }



  bool mtime_token = false;
  //for -mtime
  auto mtime_iter = std::find(arg_list.begin(),arg_list.end(),"-mtime");
  //TODO: print to error & exit gracefully
  if (mtime_iter != arg_list.end())
  {
    if (++mtime_iter == arg_list.end())
    {
      std::cout << "find: missing argument to `-mtime'" << std::endl;
      return 1;
    }
    --mtime_iter;
    // not working?
    if (arg_list[std::distance(arg_list.begin(),mtime_iter)].compare("0") != 0)
    {
      std::cout << "find: invalid argument `" << arg_list[std::distance(arg_list.begin(),mtime_iter)+1] << "' to `-mtime'" << std::endl;
      return 1;
    }
    mtime_token = (mtime_iter != arg_list.end()) ? true : false;
  }


  //for -type
  //TODO: WHAT IF THERE ARE MULTIPLE '-type's !?!?!?!? <- vector of types!!!!!
  //we cant use std::find
  //auto type_iter = std::find(arg_list.begin(),arg_list.end(),"-type");
  //char type_token = (type_iter != arg_list.end()) ? arg_list[type_iter+1] : (char) 0;

  //for -L
  fs::directory_options L_token = (std::find(arg_list.begin(),arg_list.end(),"-L") != arg_list.end()) ?
    fs::directory_options::follow_directory_symlink : fs::directory_options::none;

  //printing out the initial directory because recursive_directory_iterator always skips it
  //TODO: this is a hack
  if (name_token == "" || name_token == ".") {
      std::cout << "." << std::endl;
  }

  //TODO: only start at "." if startpath unspecified 
  for (auto& p : fs::recursive_directory_iterator(".",L_token))
  {
    //this is for print
    std::string dir_entry_printable = ((std::string)p.path()).substr(1,((std::string)p.path()).length()-1);

    //for -name
    std::string name_token_comp = name_token;
    if (name_token == "") name_token_comp = *(--p.path().end());

    //for -mtime

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::time_t mtime_token_comp = now;
    if (mtime_token) mtime_token_comp = std::chrono::system_clock::to_time_t(fs::last_write_time(p.path()));


    //The Holy Mr. If Statment
    if (
      (*(--p.path().end())).compare(name_token_comp) == 0 &&
      mtime_token_comp >= (now - 86400) // &&
      // type qualification
    ) std::cout << "." << dir_entry_printable << '\n';
  }

  // main_loop_r(fs::current_path(),L_token,name_token,mtime_token);

  return 0;
}
