#include <iostream>
#include <vector>

struct A
{
    A() = delete;
    A(int) { }
};

int main()
{
    std::vector<A> f;

    f.reserve(100);

    //f.resize(100); // COMPILE FAILURE

    f.resize(100, A(1));

    f.push_back(A(1));
    f.emplace_back(A(1));
    
    //std::vector<A> g(100); // COMPILE FAILURE
    std::vector<A> g(100, A(1)); 
}
