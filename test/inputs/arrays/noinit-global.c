#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char dest[7];
int main() {
    char source[] = "noinit";
    memcpy(dest, source, sizeof dest);
}

