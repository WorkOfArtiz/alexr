#include "alexr.h"

#define input " - 3 - 44 - 55 -  hi_there           +--+>> \"hi there\" <<3 4"

int main(void) {
    tokstream_t *ts = str2ts(input);
    char        *tok;

    printf("parsing: '%s'\n", input);

    FOR_TOK(ts, tok) {
        printf("token(%s)\n", tok);
    }

    plerror(stdout);
}