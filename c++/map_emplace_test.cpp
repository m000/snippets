#include<cstdint>
#include<iostream>
#include<string>
#include<map>
#if 0
Demo on how to use the emplace method of C++ ordered maps for optimal
performance in insertions of objects.

Two possible naive approaches are:
    - Use map.find() to identify if an object is present and then using
    map.insert() to insert the element. The problem here is that two lookups are
    performed: one for map.find() and one internally by map.insert() in order to
    find the insertion position.
    - Use map.emplace() to directly insert the value without a separate lookup.
    We can test if the insertion was successful by checking the returned value.
    The problem here is that in case where the insertion fails, a emplace will
    nevertheless create and then discard a new object.

Instead, the approach shown here uses map.lower_bound() to get the position
nearest to the eventual position of the inserted element. If the element already
exists there, no further action is required. In case we need to insert the
element, we now have a hint to provide to map.emplace_hint() in order to make
the insertion in constant time.

Compile:
    g++ -std=c++11 map_emplace_test.cpp
#endif

class map_value_t {
   public:  
    std::string s;
    uint32_t v = 0;

    map_value_t(std::string s, uint32_t v): s(s), v(v) {
        std::cerr << "new:" << this << "\t" << v << ":" << s << std::endl;
    }
    ~map_value_t() {
        std::cerr << "del:" << this << "\t" << v << ":" << s << std::endl;
    }
};

typedef std::map<std::string, map_value_t> map_t;

int main() {
    map_t m;
    uint32_t v;

    v = 1;
    {
        auto emplaced = m.emplace(std::piecewise_construct,
            std::forward_as_tuple("o1"),
            std::forward_as_tuple("o1", v));
        map_value_t &mv = emplaced.first->second;
        std::cout << emplaced.second << ":" << &mv << "\t" <<
            mv.v << ":" << mv.s << std::endl;
    }

    std::cout << "------------------------------" << std::endl;

    // this creates a temporary object which is discarded because value for "o1" exists
    v = 2;
    {
        auto emplaced = m.emplace(std::piecewise_construct,
            std::forward_as_tuple("o1"),
            std::forward_as_tuple("o1", v));
        map_value_t &mv = emplaced.first->second;
        std::cout << emplaced.second << ":" << &mv << "\t" <<
            mv.v << ":" << mv.s << std::endl;
    }

    std::cout << "------------------------------" << std::endl;

    // this avoids creating a temporary object and paying the lookup cost for "o1" twice
    // not applicable for unordered_map!
    v = 3;
    {
        auto m_it = m.lower_bound("o1");
        bool found = (m_it != m.end() && m_it->first == "o1");
        if (!found) {
            m_it = m.emplace_hint(m_it, std::piecewise_construct,
                std::forward_as_tuple("o1"),
                std::forward_as_tuple("o1", v));
        }
        map_value_t &mv = m_it->second;
        std::cout << !found << ":" << &mv << "\t" <<
            mv.v << ":" << mv.s << std::endl;
    }

    std::cout << "------------------------------" << std::endl;
}

// vim: et:ts=4:sts=4:sw=4:
