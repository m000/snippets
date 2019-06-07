#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#if 0
Demo showing the move semantics for C++ strings.

We use a class containing a string member to make this example slightly more
useful. We also include an example of how move semantics cannot be used with
C strings.

Compile:
    g++ -std=c++11 move_semantics.cpp -o move_semantics
#endif

// #### Data types ###################################################
// value type with inline string
class map_valuei_t {
   public:
    uint32_t v = 0;
    std::string s;

    map_valuei_t(std::string s, uint32_t v): s(std::move(s)), v(v) {
        std::cerr << "new:" << *this << std::endl;
    }
    ~map_valuei_t() {
        std::cerr << "end:" << *this << std::endl;
    }
    friend std::ostream &operator<<(std::ostream &o, const map_valuei_t &t);
};
std::ostream& operator<<(std::ostream &o, const map_valuei_t &t) {
    o << "\t" << t.v << ":" << t.s << std::endl <<
         "\toaddr:\t" << &t << std::endl <<
         "\tsaddr:\t" << &t.s << std::endl <<
         "\tsbuf:\t" << (void *)(t.s.c_str());
    return o;
}

// overload string << operator for strings to dump address
std::ostream& operator<<(std::ostream &o, const std::string &s) {
    o << "string:\t" << s.c_str() << std::endl <<
        "\taddr:\t" << &s << std::endl <<
        "\tbuf:\t" << (void *)(s.c_str());
    return o;
}



// #### Testing code #################################################
int main() {
    // ---- part 1 ---------------------------------------------------
    std::cout << "---string to string-----------" << std::endl;
    std::string s("123456789012345678901234567890");  // a string
    const char *sbuf = s.c_str();  // pointer to the string buffer

    std::cout << s << std::endl;       // print string information (overloaded)
    map_valuei_t v1(s, 1);             // new object - string is copied
    map_valuei_t v2(std::move(s), 2);  // new object - string contents moved

    // modify v1.s and v2.s to demonstrate that sbuf is now used by v2.s
    v1.s[0] = '*';
    v2.s[1] = '*';
    std::cout << "cstr:\t" << sbuf << std::endl
              << "\taddr:\t" << (void *)sbuf << std::endl;

    // show that s now is empty and uses another buffer
    std::cout << s << std::endl;

    // ---- part 2 ---------------------------------------------------
    // move semantics can't be used with C strings
    std::cout << "---cstr to string-------------" << std::endl;
    char *cstr = strdup("hello world! hello world! hello world! hello world!");
    std::cout << "cstr:\t" << cstr << std::endl
              << "\taddr:\t" << (void *)cstr << std::endl;

    std::string s1(cstr);
    std::cout << s1 << std::endl;

    std::string s2(std::move(cstr));
    std::cout << s2 << std::endl;

    // ---- the end --------------------------------------------------
    free(cstr);
    std::cout << "---auto cleanup---------------" << std::endl;
}

// vim: et:ts=4:sts=4:sw=4:
