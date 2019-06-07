#include <cassert>
#include <cstdint>
#include <cstring>
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
In the first example on this subject (map_refwrap.cpp) the map and the inserted
objects all lived in the scope of main. We expand this example to show how how
objects can be inserted out of the scope of the map. The problem with this
approach is that deallocation has to be handled manually. I.e., when an object
is erased from the map, it will not be automatically deallocated.

Compile:
    g++ -std=c++11 map_reference_wrapper2.cpp -o map_reference_wrapper2
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
#if 0
    map_value_t(map_value_t&& other) {
        s = std::move(other.s);
        v = other.v;
        std::cerr << "mov" << *this << std::endl;
    }
#endif
    ~map_value_t() {
        std::cerr << "end:" << *this << std::endl;
    }
    friend std::ostream &operator<<(std::ostream &o, const map_value_t &t);
};
std::ostream& operator<<(std::ostream &o, const map_value_t &t) {
    o << "\t" << t.v << ":" << t.s << std::endl <<
         "\toaddr:\t" << &t << std::endl <<
         "\tsaddr:\t" << &t.s << std::endl <<
         "\tsbuf:\t" << (void *)(t.s.c_str());
    return o;
}

// overload string << operator to dump address
std::ostream& operator<<(std::ostream &o, const std::string &s) {
    o << "string:\t" << s.c_str() << std::endl <<
        "\taddr:\t" << &s << std::endl <<
        "\tbuf:\t" << (void *)(s.c_str());
    return o;
}

#if defined(MAP_REFVAL)
// Map. The value type is a reference.
typedef std::unordered_map<std::reference_wrapper<std::string>,
                           map_value_t&,
                           std::hash<std::string>,
                           std::equal_to<std::string>>
    map_t;
#else
// Map. The value type is not a reference. This allows emplacing objects.
typedef std::unordered_map<std::reference_wrapper<std::string>,
                           map_value_t,
                           std::hash<std::string>,
                           std::equal_to<std::string>>
    map_t;
#endif


// #### Testing code #################################################
const char *get_string() {
    static const char *strings[] = {
        "the whistlings of death been hangin all around my door",
        "the wave we once rode together has crashed down upon this deserted shore",
        "I felt so lost and I felt so scared but I can tell that you don't care",
        "I've been burned and I've been fooled, don't need love and and don't need you",
        "got no solution but a revolution",
        nullptr
    };
    int n, c;
    do {
        n = 0;
        c = -1;
        std::cout << std::endl << "Make a choice:" << std::endl;
        while (strings[n] != nullptr) {
            std::cout << "\t[" << n << "] " << strings[n] << std::endl;
            n++;
        }
        std::cout << "Choice? " << std::flush;
        std::cin >> c;
    } while (std::cin && !(c >= 0 && c < n));
    return strings[c];
}

void map_dump(const map_t &m) {
    for (auto &m_it : m) {
        const std::string &mk = m_it.first;
        const map_value_t &mv = m_it.second;
        std::cout << mk << std::endl;
        std::cout << "obj" << mv << std::endl;
        std::cout << std::endl;
    }
}

const char *add_one(map_t &m, uint32_t n) {
    const char *k = get_string();
#if defined(MAP_REFVAL)
    // this works, but allocated objects aren't automatically freed
    map_value_t *mv = new map_value_t(k, n);
    m.insert(std::make_pair(std::ref((*mv).s), std::ref(*mv)));
#else
    map_value_t mv(k, n);
    //m.insert(std::make_pair(std::ref(mv.s), std::move(mv)));
    //map_value_t mv(k, n);
    m.emplace(std::piecewise_construct,
            std::forward_as_tuple(std::ref(mv.s)), 
            std::forward_as_tuple(std::move(mv)));
#endif
    return k;
}

int main() {
    map_t m;

#if defined(MAP_REFVAL)
    std::cout << "---test insert----------------";
    const char *ck1 = add_one(m, 1);
    const char *ck2 = add_one(m, 2);
    const char *ck3 = add_one(m, 2);

    std::cout << "---dump-----------------------" << std::endl;
    map_dump(m);
    
    std::cout << "---delete 1-------------------" << std::endl;
    std::string k1(ck1);
    auto m_it1 = m.find(k1);
    assert(m_it1 != m.end());
    std::cout << "obj" << m_it1->second << std::endl;
    //delete &m_it1->second;  // we need delete to release obj
    m.erase(m_it1);

    std::cout << "---delete 2-------------------" << std::endl;
    std::string k2(ck2);
    auto m_it2 = m.find(k2);
    assert(m_it2 != m.end());
    std::cout << "obj" << m_it2->second << std::endl;
    delete &m_it2->second;  // we need delete to release obj
    m.erase(m_it2);

    std::cout << "---dump-----------------------" << std::endl;
    map_dump(m);

    std::cout << "---auto cleanup---------------" << std::endl;
#else

#endif
}

// vim: et:ts=4:sts=4:sw=4:
