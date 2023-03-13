// struct declarations
struct s_person;
struct u_value;

// type definitions
typedef unsigned int(* typeDef_0)(unsigned int, unsigned int);
typedef void(* typeDef_1)(unsigned int);

// struct definitions
struct s_person {
    unsigned char structVar0[50];
    unsigned int structVar1;
};
struct u_value {
    unsigned int structVar2;
};

// function declarations
unsigned int add(unsigned int var0, unsigned int var1);
int main(void);

// global variable definitions
unsigned char s1[6] = {104,101,108,108,111,0,};
unsigned int a[5] = {1,2,3,4,5,};
unsigned char c = 0;
unsigned char uc = 0;
unsigned short s = 0;
unsigned short us = 0;
unsigned int i = 0;
unsigned int ui = 0;
unsigned long l = 0;
unsigned long ul = 0;
unsigned long ll = 0;
unsigned long ull = 0;
float f = 0.0E+0;
double d = 0.0E+0;
long double ld = 0.0E+0;
unsigned int* ip = 0;
unsigned char _str[6] = {104,101,108,108,111,0,};
unsigned char* cp = 0;
unsigned char* vp2 = 0;
typeDef_0 func_ptr = 0;
struct s_person p = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0,};
unsigned char _str_1[50] = {74,111,104,110,32,68,111,101,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
struct u_value v = {0,};
typeDef_0 sub = 0;
unsigned char* vp = 0;
typeDef_1 fp = 0;

unsigned int add(unsigned int var0, unsigned int var1){
    unsigned int a;
    unsigned int b;
    block0:
    a = var0;
    b = var1;
    return ((int)a) + ((int)b);
}

int main(void){
    unsigned int var0;
    struct s_person var1;
    struct u_value var2;
    block0:
    var0 = 0;
    c = 97;
    uc = 98;
    s = 32767;
    us = -1;
    i = 2147483647;
    ui = -1;
    l = 9223372036854775807;
    ul = -1;
    ll = 9223372036854775807;
    ull = -1;
    f = 3.141590118408203125E+0;
    d = 3.1415926535897931159979634685442E+0;
    ld = 3.1415926535897932385128089594062E+0;
    ip = 0;
    cp = (&(_str[0]));
    vp2 = 0;
    func_ptr = add;
    memcpy((unsigned char*)(&(var1.structVar0)), &(_str_1[0]), 50);
    (var1.structVar1) = 30;
    memcpy(&((p.structVar0)[0]), (unsigned char*)(&var1), 56);
    (*((unsigned int*)(&var2))) = 42;
    v = var2;
    sub = add;
    return 0;
}

