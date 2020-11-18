#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

fs::path START_PATH = fs::current_path();


int main(int argc, char **argv) {
  fs::directory_options.follow_directory_symlink = false;


  std::vector<std::string> arg_list;
  for (int i = 1; i < argc; i++) arg_list.push_back((std::string)argv[i]);



  std::cout<<".\n";
  for(auto& p: fs::recursive_directory_iterator(".")){
      auto s = (std::string) p.path();
      std::string p_s = s.substr(1,s.length()-1);
      std::cout <<"." <<p_s << '\n';
    }
  return 0;
}
