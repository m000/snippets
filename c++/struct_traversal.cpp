#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iostream>
#if 0
This is a demo for offsets-based struct traversal in C++.
The test structs defined here are meant for demonstration purposes.
The functions are meant to be used for traversing structs inside memory blobs,
where type information is not available.
#endif

template <typename T>
void struct_get(T *v, void *ptr, off_t offset) {
    memcpy(v, (uint8_t *)ptr+offset, sizeof(T));
}

template <typename T>
void struct_get(T *v, void *ptr, std::initializer_list<off_t> member_offsets) {
    auto it = member_offsets.begin();
    auto o = *it;
    while (true) {
        it++;
        if (it == member_offsets.end()) break;
        struct_get(&ptr, ptr, o);
        o = *it;
    }
    struct_get(v, ptr, o);
}

off_t ptrsub(void *a, void *b) {
    return (uint8_t *)b - (uint8_t *)a;
}

struct s2 {
    uint64_t a;
    uint16_t b;
    uint32_t c;
    uint32_t d;
};

struct s1 {
    uint8_t a;
    uint64_t b;
    struct s2 *c;
    uint32_t d;
};

struct s0 {
    int32_t a;
    struct s1 *b;
    int32_t c;
};

int main() {
    struct s2 _s2= { 1, 2, 3, 4 };
    struct s1 _s1 = { 1, 2, &_s2, 4 };
    struct s0 _s0 = { 1, &_s1, 3 };

    // v0 = _s0.c
    decltype(_s0.c) v0;
    struct_get(&v0, &_s0, ptrsub(&_s0, &_s0.c));
    std::cout << v0 << std::endl << std::endl;

    // v1 = _s0.c
    decltype(_s0.c) v1;
    struct_get(&v1, &_s0, {ptrsub(&_s0, &_s0.c)});
    std::cout << v1 << std::endl << std::endl;

    // v2 = _s0.b->c->d
    decltype(_s0.b->c->d) v2;
    struct_get(&v2, &_s0, {
            ptrsub(&_s0, &_s0.b),
            ptrsub(&_s1, &_s1.c),
            ptrsub(&_s2, &_s2.d),
    });
    std::cout << v2 << std::endl << std::endl;

    // v3 = _s0.b->c->c
    decltype(_s0.b->c->c) v3;
    struct_get(&v3, &_s0, {
            ptrsub(&_s0, &_s0.b),
            ptrsub(&_s1, &_s1.c),
            ptrsub(&_s2, &_s2.c),
    });
    std::cout << v3 << std::endl << std::endl;
}
