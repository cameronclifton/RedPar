#include <utility>
     void foo(int& a, int& b) { a = std::move(b); }
int main() { return 0; };