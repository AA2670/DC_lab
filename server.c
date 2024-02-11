#include "add.h"
#include <stdio.h>
#include <stdlib.h>

int *add_1_svc(numbers *argp, struct svc_req *rqstp) {
    static int result;

    printf("add(%d,%d) is called\n", argp->a, argp->b);

    result = argp->a + argp->b;

    // Ensure the memory for 'result' persists beyond the function call
    int *res = (int *)malloc(sizeof(int));
    if (res == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    *res = result;

    return res;
}
