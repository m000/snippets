#include <iostream>
#include <algorithm>
#define ARRAY_MAX 100

int main(int argc, char *argv[]) {
    int n;
    int a[ARRAY_MAX];

    // read integers
    for (n=0; (std::cin >> a[n]) && (n<ARRAY_MAX); n++) {}

    // print
    for (int i=0; i<n; i++) { std::cout << a[i] << ":"; }
    std::cout << std::endl;

    // sort
    std::sort(a, a+n);

    // print
    for (int i=0; i<n; i++) { std::cout << a[i] << ":"; }
    std::cout << std::endl;
}

// vim: et:ts=4:sts=4:sw=4:
