#include <filesystem>

namespace fs = std::filesystem;

int main() {
  fs::path p = fs::current_path();
}
