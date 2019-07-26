
// struct declarations
struct s_test;
struct s_arrptr;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_test {
    unsigned int structVar0;
};
struct s_arrptr {
    struct s_test (*structVar1)[1];
};

// function declarations
int main(int var0, char** var1);
unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

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
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    if (var3 != 2) {
        var2 = -1;
        return var2;
    } else {
        var6 = strtol(*(((unsigned char**)(var4)) + 1), &var5, 10);
        if (((unsigned int)(*var5)) != 0) {
            var2 = -1;
            return var2;
        } else {
            (var7.structVar0) = ((unsigned int)var6);
            memcpy((unsigned char*)(&(var8[0])), (unsigned char*)(&var7), 4);
            (var9.structVar1) = ((struct s_test (*)[1])(&(var8[0])));
            var2 = (((*(var9.structVar1))[0]).structVar0);
            return var2;
        }
    }
}


