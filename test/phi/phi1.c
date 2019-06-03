#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *p;
    int v = strtol(argv[1], &p, 10);
    int g = 0;
    switch(v) {
    case 1:
        g = 43;
        break;
    case 2:
        g = 42;
        break;
    case 3:
        g = 44;
        break;
    case 4:
        g = 30;
        break;
    case 5:
        g = 21;
        break;
    case 6:
        g = 40;
        break;
    case 7:
        g = 0;
        break;
    case 8:
        g = 70;
        break;
    case 9:
        g = 11;
        break;
    case 10:
        g = 1;
        break;
    }
    return g;
}
