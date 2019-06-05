#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#if 0
Demo for using reference wrapper keys in C++ maps in order to avoid redundant
copying of keys.

It is not allowed to use references as the key type in C++ maps. Going with
this behaviour means that there may be a fair amount of unneeded copying when
populating the maps. Reference wrappers allow us to overcome this limitation.
This is useful when the key we want to use for the map type is already contained
inside the value type.

The demonstrated behaviour can be observed by checking the saddr and kaddr
values in the dump phase.

Compile with reference wrappers (avoids key copying):
    g++ -std=c++11 map_refwrap.cpp -o map_refwrap

Compile without reference wrappers (keys are copied on insertions):
    g++ -std=c++11 -DUSE_SIMPLE_MAP map_refwrap.cpp -o map_refwrap
#endif

// #### Data types ###################################################
// value type with inline string
class map_value_t {
   public:
    uint32_t v = 0;
    std::string s;

    map_value_t(std::string s, uint32_t v): s(s), v(v) {
        std::cerr << "new:" << *this << std::endl;
    }
    ~map_value_t() {
        std::cerr << "end:" << *this << std::endl;
    }
    friend std::ostream &operator<<(std::ostream &o, const map_value_t &t);
};
std::ostream& operator<<(std::ostream &o, const map_value_t &t) {
    o << "\t" << t.v << ":" << t.s << std::endl <<
         "\toaddr:\t" << &t << std::endl <<
         "\tsaddr:\t" << &t.s;
    return o;
}

#if !defined(USE_SIMPLE_MAP)
typedef std::unordered_map<std::reference_wrapper<std::string>,
                           map_value_t&,
                           std::hash<std::string>,
                           std::equal_to<std::string>>
    map_t;
#else
typedef std::map<std::string, map_value_t&> map_t;
#endif



// #### Testing code #################################################
int main() {
    map_t m;

    std::cout << "---construct------------------" << std::endl;
    map_value_t mv1("foo", 1);
    map_value_t mv2("bar", 1);

    std::cout << "---forward--------------------" << std::endl;
    m.insert(std::make_pair(std::ref(mv1.s), std::ref(mv1)));
    m.insert(std::make_pair(std::ref(mv2.s), std::ref(mv2)));

    std::cout << "---dump-----------------------" << std::endl;
    for (auto &m_it : m) {
        const std::string &mk = m_it.first;
        map_value_t &mv = m_it.second;
        std::cout << mk << mv << std::endl <<
            "\tkaddr:\t" << &mk << std::endl;
    }

    std::cout << "---auto cleanup---------------" << std::endl;
}

// vim: et:ts=4:sts=4:sw=4:
