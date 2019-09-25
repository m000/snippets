#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REGEX_LEN 128
#define LIB_BASEADDR_RE_FMT "([0-9a-f]+)"  "-"  "([0-9a-f]+)"  "[^/]+"  ".*"  "([^/]*%s-[0-9.]+[^/]+)"
#define LIB_BASEADDR_RE_NSUB 4  /* the number of subexpressions + 1 */

int main(int argc, char *argv[]) {
    regex_t re;
    char re_string[REGEX_LEN];
    FILE *map;
    char *line = NULL;
    size_t line_n = 0;
    int status;
    regmatch_t matches[LIB_BASEADDR_RE_NSUB];
    long long addr;


    /* create regex for the lib we search for */
    if (snprintf(re_string, REGEX_LEN*sizeof(char), LIB_BASEADDR_RE_FMT, argv[1]) > REGEX_LEN) {
        fprintf(stderr, "RegEx truncated. Increase REGEX_LEN.\n");
        exit(1);
    }

    /* compile regex */
    if (regcomp(&re, re_string, REG_EXTENDED|REG_ICASE)) {
        fprintf(stderr, "RegEx failed to compile: \"%s\".\n", re_string);
        exit(1);
    }

    /* open file and start reading */
    if ((map = fopen("/proc/self/maps", "r")) == NULL) {
        fprintf(stderr, "Failed to open maps file.\n");
        exit(1);
    }

    while(getline(&line, &line_n, map) > 0) {
        /* execute regex */
        memset(matches, 0, sizeof(matches));
        status = regexec(&re, line, LIB_BASEADDR_RE_NSUB, matches, 0);

        /* no match */
        if (status != 0) continue;

        printf("%s\n", re_string);

        printf("%d\n", matches[2].rm_so);
        printf("%s", &line[matches[2].rm_so]);
        printf("%d\n", matches[3].rm_so);
        printf("%s", &line[matches[3].rm_so]);
        /* break on first match */
        break;
    }

    /* matches[1] contains the first sub-match */
    if (matches[1].rm_so == -1) {
        fprintf(stderr, "WTF?\n");
        exit(1);
    }
   
    /* terminate the line after the first match and parse string */
    line[matches[1].rm_eo] = '\0';
    addr = strtoll(&line[matches[1].rm_so], NULL, 16);

    /* print shit */
    printf("%p\n", (void *)addr);

    /* cleanup */
    free(line);
    regfree(&re);
    fclose(map);

    return 0;
}

/*:vim: etts=4:sts=4:sw=4 :*/
