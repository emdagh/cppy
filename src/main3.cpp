#include <vector>
#include "zip.hpp"
#include <iostream>

using wee::zip;

int main(int argc, char **argv) {
    auto a = std::vector<int> { 0, 1, 2 };
    auto b = std::vector<std::string> { "one", "two", "three" };

    for(auto&& [x, y] : zip(a, b)) {
        std::cout << x << " " << y << std::endl;
    }
  return 0;
}
