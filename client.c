#include <stdio.h>
#include <stdlib.h>
#include "add.h" // Include the necessary header file with function prototypes and structure definition

void add_prog_1(char *host, int x, int y) {
    CLIENT *clnt;
    int *result_1;
    numbers add_1_arg;

#ifndef DEBUG
    clnt = clnt_create(host, ADD_PROG, ADD_VERS, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(host);
        exit(1);
    }
#endif /* DEBUG */

    add_1_arg.a = x;
    add_1_arg.b = y;

    // Call the remote procedure 'add_1'
    result_1 = add_1(&add_1_arg, clnt);
    if (result_1 == NULL) {
        clnt_perror(clnt, "call failed");
    } else {
        printf("Result: %d\n", *result_1);
    }

#ifndef DEBUG
    clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[]) {
    char *host;

    if (argc < 4) {
        printf("usage: %s server_host number1 number2\n", argv[0]);
        exit(1);
    }

    host = argv[1];
    add_prog_1(host, atoi(argv[2]), atoi(argv[3]));

    exit(0);
}
