
// struct declarations
union u1;
union u2;

// anonymous struct declarations

// type definitions

// struct definitions

// union definitions
union u1 {
    unsigned char* ty0;
    unsigned char (*ty1)[5];
};
union u2 {
    unsigned char (*ty0)[5];
    unsigned char* ty1;
};

// function declarations
unsigned char (*get_me(void))[5];
int main(int var0, char** var1);
unsigned char* malloc(unsigned long var0);

// global variable definitions

unsigned char (*get_me(void))[5]{
    unsigned char* var0;
    union u1 var1;
    block0: ;
    var0 = malloc(5);
    (var1.ty0) = var0;
    return var1.ty1;
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    unsigned char* var5;
    union u2 var6;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    (var6.ty0) = get_me();
    var5 = (var6.ty1);
    return (unsigned int)var5;
}


