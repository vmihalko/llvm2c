// struct declarations
union u1;

// union definitions
union u1 {
    unsigned char* ty0;
    unsigned char (*ty1)[5];
};

// function declarations
unsigned char (*get_me(void))[5];
int main(int var0, char** var1);
extern unsigned char* malloc(unsigned long var0);

unsigned char (*get_me(void))[5]{
    unsigned char* me;
    union u1 var1;
    block0:
    me = malloc(5);
    (var1.ty0) = me;
    return var1.ty1;
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* s;
    union u1 var6;
    block0:
    var2 = 0;
    argc = var0;
    argv = var1;
    (var6.ty1) = get_me();
    s = (var6.ty0);
    return (unsigned int)s;
}

