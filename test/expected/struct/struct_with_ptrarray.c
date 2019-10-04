
// struct declarations
struct s_test;
struct s_arrptr;
union u1;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_test {
    unsigned int structVar0;
};
struct s_arrptr {
    struct s_test (*structVar1)[1];
};

// union definitions
union u1 {
    struct s_test (*ty0)[1];
    struct s_test* ty1;
    unsigned char* ty2;
};

// function declarations
int main(int var0, char** var1);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    unsigned char* var5;
    unsigned long var6;
    struct s_test var7;
    struct s_test var8[1];
    struct s_arrptr var9;
    union u1 var10;
    union u1 var11;
    union u1 var12;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    if (var3 != 2) {
        var2 = -1;
        return var2;
    } else {
        var6 = strtol(*(((unsigned char**)(var4)) + 1), &var5, 10);
        if (((int)(*var5)) != 0) {
            var2 = -1;
            return var2;
        } else {
            (var7.structVar0) = ((unsigned int)var6);
            (var10.ty1) = (&(var8[0]));
            (var11.ty1) = (&var7);
            memcpy(var10.ty2, var11.ty2, 4);
            (var12.ty1) = (&(var8[0]));
            (var9.structVar1) = (var12.ty0);
            var2 = (((*(var9.structVar1))[0]).structVar0);
            return var2;
        }
    }
}


