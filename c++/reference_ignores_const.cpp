#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t a[2][8];
    int x;
    int y;
    int *p;
} S;

void foo(const S *s, const int *i) {
    // a[*i] copied to local
    uint32_t v1 = *(uint32_t *)(s->a[*i]);

    // a[*i] as reference using c-style casting
    // const specifier silently discarded when computing the rvalue
    uint32_t &v2 = *(uint32_t *)(s->a[*i]);

    // a[*i] as const reference using c-style casting
    // const specifier is reapplied
    const uint32_t &v3 = *(uint32_t *)(s->a[*i]);

    // a[*i] as reference using using reinterpret_cast
    // const specifier won't be discarded!
    // uint32_t &v4 = * reinterpret_cast<uint32_t *>(s->a[*i]);

    // print addresses
    printf("%d: %p %p %p %p\n", __LINE__, s->a[*i], &v1, &v2, &v3);

    // print values - 1 513 513 513
    printf("%d: %u %u %u %u\n", __LINE__, *s->a[*i], v1, v2, v3);
    
    //s->a[*i][0] = 4; // not allowed due to const specifier

    v1 = 520; // assigns bytes to {8, 2}
    v2 = 516; // assigns bytes to {4, 2} - const specifier defeated
    //v3 = 514; // assignes bytes to {2, 2} // not allowed due to const specifier

    // print values - 4 520 516 516
    printf("%d: %u %u %u %u\n", __LINE__, *s->a[*i], v1, v2, v3);

}

int main() {
    S s = {};
    int i = 1;

    // *(uint32_t *)(s.a[1]) = 513
    s.a[1][0] = 1;
    s.a[1][1] = 2;

    printf("%d: %u\n", __LINE__, s.a[i][0]); // prints 1
    foo(&s, &i);
    printf("%d: %u\n", __LINE__, s.a[i][0]); // prints 4 - const specifier defeated
}
