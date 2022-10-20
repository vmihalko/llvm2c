
int foo(int[*]);

void bar(int a);

///home/vmihalko/DIPLO/l/llvm2c/parser/parseMetadataTypes.cpp
struct lol {
    int pica;
} a;

union kokot {
    int llvm2c;
}c ;


int b[] = {0};

float f = 0.778;
short g = 2;
double h = 0.5;
//long double w = 0.5;

unsigned short ug = 2;
unsigned int ui = 5;

int main(int argc, char**argv) {
    if (argc == 2) {
        int unsigned * b;
        int* a = b + 1;
        b = 8;
    }

    int (*f)(unsigned int);

    int arr[4] = {1,2,3};
    int a = arr[1];

    foo(arr);
    bar(a);

    return arr[0];
}

