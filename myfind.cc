#include <fstream>
#include <iostream>
#include <filesystem>

using fs = std::filesystem;

fs::path START_PATH = fs::current_path();

//iterate throught the children of a directory
void print_dir(fs::path directory) {
  (void) directory;
}


int main(int arc, char **argv) {
  fs::current_path(fs::temp_directory_path());
  fs::create_directories("sandbox/a/b");
  std::ofstream("sandbox/file1.txt");
  fs::create_symlink("a", "sandbox/syma");
  for(auto& p: fs::recursive_directory_iterator("sandbox"))
      std::cout << p.path() << '\n';
  fs::remove_all("sandbox");
  return 0;
}
