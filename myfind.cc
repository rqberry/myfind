#include <fstream>      //needed for filesystem
#include <iostream>     //for cout
#include <vector>       //for vector
#include <filesystem>   //for the the thing that does our project for us lol
#include <algorithm>    //for std::find
#include <string>       //for compare()
#include <chrono>       //for -mtime (for chrono stuff)
#include <ctime>        // for asctime and std::localtime


namespace fs = std::filesystem;

fs::path START_PATH = fs::current_path();


int main(int argc, char **argv) {



  std::vector<std::string> arg_list;
  for (int i = 1; i < argc; i++) arg_list.push_back((std::string)argv[i]);


  //for -name
  auto name_iter = std::find(arg_list.begin(),arg_list.end(),"-name");
  //TODO: print to error for no -name specification "find: missing argument to `-name'"
  std::string name_token = (name_iter != arg_list.end()) ? arg_list[std::distance(arg_list.begin(),name_iter)+1] : "";
  //TODO: print to error
  if (name_token.substr(0,1) == "/") {
      std::cout << "find: warning: ‘-name’ matches against basenames only, but the given pattern contains a directory separator (‘/’), thus the expression will evaluate to false all the time.  Did you mean ‘-wholename’?" << std::endl;
      return 1;
  }

  //for -mtime
  //actually we don't need this since it is always 0?
  auto mtime_iter = std::find(arg_list.begin(),arg_list.end(),"-mtime");
  //TODO: print error for no -mtime specification "find: missing argument to `-mtime'"
  int mtime_token = (mtime_iter != arg_list.end()) ? std::stoi(arg_list[std::distance(arg_list.begin(),mtime_iter)+1]) +1 : -1;

  //calculate what boundary is for the times we're looking for
  //now is std::localtime(&now)


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
      std::cout<<".\n";
  }

//  list[list.end()]

  //recursive_directory_iterator iterates through all folders
  for (auto& p : fs::recursive_directory_iterator(".",L_token))
  {
      auto s = (std::string) p.path();
      std::string p_s = s.substr(1,s.length()-1);

      std::string name_token_comp = name_token;
      if (name_token == "") name_token_comp = *(--p.path().end());
      int mtime_token_comp = mtime_token;
      if (mtime_token == -1) mtime_token_comp = fs::last_write_time(p.path());
	       //std::cout << p_s << " : " << name_token << std::endl;
      if (
        (*(--p.path().end())).compare(name_token_comp) == 0 &&
        fs::last_write_time(p.path()) == mtime_token_comp //&&
        /* type qualification */
        ) std::cout << "." << p_s << '\n';

      /*
      else {
	       std::cout << "." << p_s << '\n';
      }
      */
  }
  return 0;
}
