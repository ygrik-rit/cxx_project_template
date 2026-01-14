module;
#include <iostream>

module Hello;

namespace mod {
auto Hello::operator()() const -> int {
  std::cout << "Hello modules" << '\n';
  return 42;
}
} // namespace mod