#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <iostream>
#include <regex>
#if 0
It is often useful for dynamic program analysis to read the memory layout of a
process from /proc pseudo-filesystem. This code snippet shows how to use C++
regex to read memory layout entries into a procmaps_entry_t struct.
#endif

#define PROCMAPS_MAXPATHLEN 256
#define PROCMAPS_SCANF_FMT "%" PRIx64 "-%" PRIx64 " %c%c%c%c %x %hhu:%hhu %u %s"
#define PROCMAPS_SSCANF(l, pe) sscanf(l, PROCMAPS_SCANF_FMT, &(pe.laddr), &(pe.haddr), &(pe.r), &(pe.w), &(pe.x), &(pe.s), &(pe.offset), &(pe.dev_maj), &(pe.dev_min), &(pe.inode), (pe.pathname))
#define PROCMAPS_SCANN 11

#define STACK_RE "\\[stack\\]"

typedef struct {
    uint64_t laddr;     //!< Start address of the region.
    uint64_t haddr;     //!< End address of the region.
    char r;             //!< Read flag. \c r if the region is readable.
    char w;             //!< Write flag. \c w if the region is writable.
    char x;             //!< Execute flag. \c x if the region is writable.
    char s;             //!< Shared flag. \c s if the region is shared, \c p if the region is private.
    uint32_t offset;    //!< Offset of the mapped region in the mapped resource (file etc.)
    uint8_t dev_maj;    //!< Major number of the device of the mapped resource.
    uint8_t dev_min;    //!< Minor number of the device of the mapped resource.
    uint32_t inode;     //!< Inode of the mapped resource on the device it resides.
    char pathname[PROCMAPS_MAXPATHLEN]; //!< File or name of the resource mapped to the region.
} procmaps_entry_t;

int mmap_grep(const char *mapfile, const char *name_re) {
    FILE *f = nullptr;
    char *mapline = nullptr;
    int mapline_len = 0;
    int nfound = 0;
    procmaps_entry_t pe;

    std::regex name_re_compiled(name_re);

    if ( (f = fopen(mapfile, "r")) == nullptr ) {
        throw std::runtime_error("could not open process map path");
    }

    while (getline(&mapline, (size_t *)&mapline_len, f) != -1) {
        if (PROCMAPS_SSCANF(mapline, pe) < PROCMAPS_SCANN) {
            continue;
        }
        if (! regex_match(pe.pathname, name_re_compiled)) {
            continue;
        }
        std::cout << mapline;
        nfound++;
    }

    if (mapline != nullptr) { free(mapline); mapline = nullptr; }
    fclose(f);

    return nfound;
}

int main(int argc, char *argv[]) {
    std::string re;
    const char *infile = nullptr;

    if (argc > 1) {
        infile = argv[1];
    } else {
#if defined(__linux__)
        infile = "/proc/self/maps";
#else
        std::cerr << "Error. Please specify an input file to read the memory layout from." << std::endl;
        exit(1);
#endif
    }

    std::cout << "Printing matches for: " << STACK_RE << std::endl;
    mmap_grep(infile, STACK_RE);
    std::cout << std::endl;

    std::cout << "Try your own regular expressions. Input file is: " << infile << "." << std::endl;
    std::cout << std::endl;

    while (!std::cin.fail()) {
        std::cout << "regex? ";
        std::cin >> re;
        if (std::cin.fail()) {
            std::cout << std::endl;
            break;
        }
        std::cout << "Printing matches for: " << re << std::endl;
        mmap_grep(infile, re.c_str());
        std::cout << std::endl;
    }
}

// vim: et:ts=4:sts=4:sw=4:
