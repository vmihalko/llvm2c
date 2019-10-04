// struct declarations
struct s_test;
struct s_arrptr;
union u1;

// struct definitions
struct s_test {
    unsigned int structVar0;
};
struct s_arrptr {
    struct s_test (*structVar1)[1];
};

// union definitions
union u1 {
    unsigned char* ty0;
    struct s_test* ty1;
    struct s_test (*ty2)[1];
};

// function declarations
int main(int var0, char** var1);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
    struct s_test t;
    struct s_test arr[1];
    struct s_arrptr aptr;
    union u1 var10;
    union u1 var11;
    union u1 var12;
    block0: ;
    var2 = 0;
    argc = var0;
    argv = var1;
    if (argc != 2) {
        var2 = -1;
        return var2;
    } else {
        num = strtol(*(((unsigned char**)(argv)) + 1), &p, 10);
        if (((int)(*p)) != 0) {
            var2 = -1;
            return var2;
        } else {
            (t.structVar0) = ((unsigned int)num);
            (var10.ty1) = (&(arr[0]));
            (var11.ty1) = (&t);
            memcpy(var10.ty0, var11.ty0, 4);
            (var12.ty1) = (&(arr[0]));
            (aptr.structVar1) = (var12.ty2);
            var2 = (((*(aptr.structVar1))[0]).structVar0);
            return var2;
        }
    }
}

