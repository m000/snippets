#include <iostream>
#include <map>

// map.lower_bound() returns the first map item with key bigger than the lookup.
// With some additional code, this can be used for efficient range lookups.

void search(std::map<int,char> &mymap, int k) {
    auto it = mymap.lower_bound(k);
    std::cout <<
    ((k == it->first) ? "found" : "not found") <<
    ((it == mymap.end()) ? " - end" : "") <<
    ((it == mymap.begin()) ? " - begin" : "") <<
    std::endl;

    if (k == it->first) return;

    if (it == mymap.begin()) return;

    if (it != mymap.end())
    std::cout << "\t     lower_bound: " << "map[" << it->first << "] = " << it->second << std::endl;

    --it;

    std::cout << "\tlower_bound_prev: " << "map[" << it->first << "] = " << it->second << std::endl;
}

int main () {
    std::map<int,char> mymap;
    int idx;

    mymap[10]='a';
    mymap[20]='b';
    mymap[30]='c';
    mymap[40]='d';
    mymap[50]='e';

    while (!std::cin.fail()) {
        std::cout << "Array:" << std::endl;
        for (auto it=mymap.begin(); it!=mymap.end(); ++it) {
            std::cout << "\t mymap[" << it->first << "] = " << it->second << std::endl;
        }

        std::cout << std::endl << "index? ";
        std::cin >> idx;
        if (std::cin.fail()) {
            std::cout << std::endl;
            break;
        }
        search(mymap, idx);
        std::cout << std::endl;
    }

    return 0;
}

// vim: et:ts=4:sts=4:sw=4:
