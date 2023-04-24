// Enumeration type
enum color { RED, GREEN, BLUE };

// Array types
char s1[] = "hello";
int a[] = {1, 2, 3, 4, 5};

// Structure type
struct person {
    char name[50];
    int age;
};
struct person p;

// Union type
union value {
    int i;
    float f;
};

// Function type
int add(int a, int b) {
    return a + b;
}

void my_function(char c, unsigned char uc, short s, unsigned short us, int i,
                 unsigned int ui, long l, unsigned long ul, long long ll, unsigned long long ull,
                 float f, double d, long double ld, void *vp, void (*fp)(int),
                 /*enum color mycolor,*/ int *ip, int (*func_ptr)(int, int),
                 char s1[], int a[], struct person p/*, union value v*/);

int main(void) {
    struct person p = (struct person){"John Doe", 30};

    my_function(
    'a',
    'b',
    32767,
    65535,
    2147483647,
    4294967295,
    9223372036854775807,
    18446744073709551615UL,
    9223372036854775807LL,
    18446744073709551615ULL,
    3.14159f,
    3.14159265358979323846,
    3.14159265358979323846L,
    0, 0, 0, 0, s1, a, p);
    return 0;
}
