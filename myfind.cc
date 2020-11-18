#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

fs::path START_PATH = fs::current_path();

//iterate throught the children of a directory
void print_dir(fs::path directory) {
  (void) directory;
}


int main(int argc, char **argv) {
  std::cout<<".\n";
  for(auto& p: fs::recursive_directory_iterator(".")){
      auto s = (std::string) p.path();
      std::string p_s = s.substr(1,s.length()-1);
      std::cout <<"." <<p_s << '\n';
    }
  return 0;
}
