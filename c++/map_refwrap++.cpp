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
The first example on this subject (map_reference_wrapper.cpp) demonstrated
how to use strings as keys for an unordered map without having them copied
when items are inserted.
In this example, we show

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

// Map. The value type is not a reference. This allows emplacing objects.
typedef std::unordered_map<std::reference_wrapper<std::string>,
                           map_value_t&,
                           std::hash<std::string>,
                           std::equal_to<std::string>>
    map_t;



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
        std::cout << std::endl << "Choice? " << std::flush;
        std::cin >> c;
    } while (std::cin && !(c >= 0 && c < n));
    return strings[c];
}

void map_dump(const map_t &m) {
    std::cout << "---dump-----------------------" << std::endl;
    for (auto &m_it : m) {
        const std::string &mk = m_it.first;
        const map_value_t &mv = m_it.second;
        std::cout << mk << std::endl;
        std::cout << "obj" << mv << std::endl;
        std::cout << std::endl;
    }
}

#if 0
const char *add_one_bad(map_t &m, uint32_t n) {
    const char *k = get_string();
    std::string *sp = new std::string(k);
 
    std::cout << *sp << std::endl;
    m.emplace(std::piecewise_construct,
            std::forward_as_tuple(std::ref(*sp)), 
            std::forward_as_tuple(std::ref(*sp), n));

    return k;
}
#endif

const char *add_one_good(map_t &m, uint32_t n) {
    const char *k = get_string();
    map_value_t *mv = new map_value_t(k, n);

    m.insert(std::make_pair(std::ref((*mv).s), std::ref(*mv)));
    return k;
}

int main() {
    map_t m;

    const char *k = add_one_good(m, 1);
    std::string ks = std::string(k);
    map_dump(m);

#if 0
    auto m_it = m.find(ks);
    assert(m_it != m.end());

    std::cout << m_it->second << std::endl;
    const char *cstr = m_it->second.s.c_str();

    std::cout << "*" << cstr << std::endl;
    m.erase(m_it);
    std::cout << "+" << cstr << std::endl;
#endif


#if 0
    s = get_string();
    map_value_t mv1(s, 1);
    s = get_string();
    map_value_t mv2(s, 2);

    //m.emplace(std::piecewise_construct,
              //std::forward_as_tuple(foo),
              //std::forward_as_tuple(foo, 4));


    std::cout << "---forward--------------------" << std::endl;
    m.insert(std::make_pair(std::ref(mv1.s), std::ref(mv1)));
    m.insert(std::make_pair(std::ref(mv2.s), std::ref(mv2)));
#endif

#if 0
        std::string *ss = new std::string(get_string());
   
    std::cout << (void *)ss << std::endl;
    //map_value_t *mv3 = new map_value_t(ss, 3);
    //m.insert(std::make_pair(std::ref((*mv3).s), std::ref(*mv3)));
    m.emplace(std::piecewise_construct,
            std::forward_as_tuple(std::ref(*ss)), 
            std::forward_as_tuple(ss, 4));

 std::cout << "---dump-----------------------" << std::endl;
    for (auto &m_it : m) {
        const std::string &mk = m_it.first;
        map_value_t &mv = m_it.second;
        std::cout << mk << mv << std::endl <<
            "\tkaddr:\t" << &mk << std::endl;
    }

    std::cout << "------------------------------" << std::endl;
    std::unordered_map<std::string, int> m2;
    std::string x("whistlings");
    char *ll = strdup("whistlings");

    std::string_view x2("xxx");

    auto m2it = m2.find(ll);

    auto foo_it = m.find(x);
    std::cout << foo_it->second.v << std::endl;
    std::cout << (void *)(& foo_it->second) << std::endl;

    std::string y("death");
    auto loo_it = m.find(std::ref(y));
    std::cout << loo_it->second.v << std::endl;
    std::cout << (void *)(& loo_it->second) << std::endl;


    std::cout << "------------------------------" << std::endl;
    get_string();
#endif
    std::cout << "---auto cleanup---------------" << std::endl;
}

// vim: et:ts=4:sts=4:sw=4:
