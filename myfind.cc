#include <fstream>      // needed for filesystem
#include <iostream>     //for cout
#include <vector>       // for vector
#include <filesystem>   // for the the thing that does our project for us lol
#include <algorithm>    //for std::find

namespace fs = std::filesystem;

fs::path START_PATH = fs::current_path();


int main(int argc, char **argv) {



  std::vector<std::string> arg_list;
  for (int i = 1; i < argc; i++) arg_list.push_back((std::string)argv[i]);



  fs::directory_options hyphen_L = (std::find(arg_list.begin(),arg_list.end(),"-L") != arg_list.end()) ?
    fs::directory_options::follow_directory_symlink : fs::directory_options::none;

  std::cout<<".\n";
  for (auto& p : fs::recursive_directory_iterator(".",hyphen_L))
  {
      auto s = (std::string) p.path();
      std::string p_s = s.substr(1,s.length()-1);
      std::cout <<"." <<p_s << '\n';
  }
  return 0;
}
