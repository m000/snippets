#include <iostream>
#include <map>
#include <cstdint>
#include <cstring>
#include <string.h>

#if 0
Structured bindings demo for C++17. 
Based on:
    https://skebanga.github.io/structured-bindings/
Compile using:
    g++-7 -std=c++17 structured-bindings.cpp    (C++17 demo)
    g++ -std=c++11 structured-bindings.cpp      (C++11 fallback)
#endif

typedef struct { uint32_t a, b, c; } mystruct_t;
typedef std::map<uint32_t, mystruct_t> mymap_t;
typedef std::tuple<mystruct_t*, mystruct_t&, bool&> mystruct_copy_t;

static inline mystruct_copy_t get_struct(mymap_t& m, uint32_t k) {
    auto it = m.find(k);
    bool exists = (it != m.end());

    if (!exists) {
        m[k] = mystruct_t();
        m[k].a = k; m[k].b = 0; m[k].c = k;
    }
    else {
        m[k].b++; m[k].c--;
    }

    mystruct_t *p = (mystruct_t *)malloc(sizeof(mystruct_t));
    memcpy(p, &m[k], sizeof(mystruct_t));

    std::cout << __FUNCTION__ << ":";
    std::cout << " p=" << (void*)p << std::flush;
    std::cout << " p->a=" << p->a;
    std::cout << " p->b=" << p->b;
    std::cout << " p->c=" << p->c;
    std::cout << (exists ? " (exists)" : " (added)") << std::endl;
    std::cout << __FUNCTION__ << ": s=" << (void *)&m[k] << std::endl;

    return std::forward_as_tuple(p, m[k], exists);
}

void process(mymap_t& m, uint32_t k, uint32_t d) {
#if __cplusplus >= 201703L
    auto [ p, s, exists ] = get_struct(m, k);
#else
    auto tuple = get_struct(m, k);
    auto& p = std::get<0>(tuple);
    auto& s = std::get<1>(tuple);
    auto& exists = std::get<2>(tuple);
#endif

    std::cout << __FUNCTION__ << ":";
    std::cout << " p=" << (void*)p << std::flush;
    std::cout << " p->a=" << p->a;
    std::cout << " p->b=" << p->b;
    std::cout << " p->c=" << p->c;
    std::cout << (exists ? " (exists)" : " (added)") << std::endl;
    std::cout << __FUNCTION__ << ": s=" << (void *)&s << std::endl;

    if (p->c - p->b <= d) {
        std::cout << "boom" << std::endl;
        s.b = 0;
        s.c = s.a;
    }

    std::cout << std::endl;
    free(p);
}

int main() {
    mymap_t M;
    process(M, 10, 4);
    process(M, 10, 4);
    process(M, 10, 4);
    process(M, 10, 4);
    process(M, 10, 4);
    process(M, 10, 4);
}

// vim: et:ts=4:sts=4:sw=4:
