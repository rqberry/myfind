#include <fstream>      //needed for filesystem
#include <iostream>     //for cout
#include <vector>       //for vector
#include <filesystem>   //for the the thing that does our project for us lol
#include <algorithm>    //for std::find
#include <string>       //for compare()

namespace fs = std::filesystem;

fs::path START_PATH = fs::current_path();


int main(int argc, char **argv) {



  std::vector<std::string> arg_list;
  for (int i = 1; i < argc; i++) arg_list.push_back((std::string)argv[i]);


  //for -name
  auto name_iter = std::find(arg_list.begin(),arg_list.end(),"-name");
  std::string name_token = (name_iter != arg_list.end()) ? arg_list[name_iter+1] : "";
  if (name_token.substr(0,1) == "/") {
      std::cout << "find: warning: ‘-name’ matches against basenames only, but the given pattern contains a directory separator (‘/’), thus the expression will evaluate to false all the time. Did you mean ‘-wholename’?" << std::endl;
      return 1;
  }

  //for -mtime
  auto mtime_iter = std::find(arg_list.begin(),arg_list.end(),"-mtime");
  int mtime_token = (mtime_iter != arg_list.end()) ? arg_list[mtime_iter+1]+1 : -1;


  //for -type
  //TODO: WHAT IF THERE ARE MULTIPLE '-type's !?!?!?!?
  //auto type_iter = std::find(arg_list.begin(),arg_list.end(),"-type");
  //char type_token = (type_iter != arg_list.end()) ? arg_list[type_iter+1] : (char) 0;

  //for -L
  fs::directory_options L_token = (std::find(arg_list.begin(),arg_list.end(),"-L") != arg_list.end()) ?
    fs::directory_options::follow_directory_symlink : fs::directory_options::none;

  //printing out the initial directory because recursive_directory_iterator always skips it
  std::cout<<".\n";

  //recursive_directory_iterator iterates through all folders
  for (auto& p : fs::recursive_directory_iterator(".",L_token))
  {
      auto s = (std::string) p.path();
      std::string p_s = s.substr(1,s.length()-1);
      if (name_token != "") {
	if (p_s.compare(name_token) == 0) std::cout << "." << p_s << '\n';
      } else {
	std::cout << "." << p_s << '\n';
      }
  }
  return 0;
}
