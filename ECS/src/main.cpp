#include "SparseSet.hpp"

int main(void)
{
    ECS::SparseSet<int> _intset(10);
    _intset.set(2, 10);
    _intset.set(3, 5);
    _intset.set(0, 1);
    _intset.set(4, 500);
    //_intset.set(25, 13);
    //_intset.set(27, 14);
    //_intset.set(26, 15);
    //_intset.set(28, 16);
    //_intset.set(30, 17);
    //_intset.set(29, 18);
    //_intset.set(31, 19);
    //_intset.set(33, 20);
    //_intset.set(32, 21);
    //_intset.set(34, 22);
    //std::cout << *(_intset.at(3)) << "\t" << *(_intset.at(25)) << std::endl;
    std::cout << _intset << std::endl;
    _intset.unset(3);
    //_intset.unset(33);
    std::cout << _intset << std::endl;
}
