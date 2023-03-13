// Integer types
char c;
unsigned char uc;
short s;
unsigned short us;
int i;
unsigned int ui;
long l;
unsigned long ul;
long long ll;
unsigned long long ull;

// Floating-point types
float f;
double d;
long double ld;

// Void type
void *vp;
void (*fp)(int);

// Enumeration type
//enum color { RED, GREEN, BLUE };
//enum color mycolor;

// Pointer types
int *ip;
void *vp2;
char *cp;
int (*func_ptr)(int, int);

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
union value v;

// Function type
int add(int a, int b) {
    return a + b;
}
int (*sub)(int, int);

int main() {
    // Assign values to global variables
    c = 'a';
    uc = 'b';
    s = 32767;
    us = 65535;
    i = 2147483647;
    ui = 4294967295;
    l = 9223372036854775807;
    ul = 18446744073709551615UL;
    ll = 9223372036854775807LL;
    ull = 18446744073709551615ULL;
    f = 3.14159f;
    d = 3.14159265358979323846;
    ld = 3.14159265358979323846L;
    //mycolor = GREEN;
    ip = 0x0;
    cp = "hello";
    vp2 = 0x0;
    func_ptr = &add;
    p = (struct person){"John Doe", 30};
    v = (union value){42};
    sub = &add;
    
    return 0;
}
