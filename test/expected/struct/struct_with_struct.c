
// struct declarations
struct s_two;
struct s_one;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_one {
    unsigned int structVar1;
};
struct s_two {
    struct s_one structVar0;
};

// anonymous struct definitions

// global variable definitions

// function declarations
int main(int var0, char** var1);
unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    unsigned char* var5;
    unsigned long var6;
    struct s_two var7;
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
            var7.structVar0.structVar1 = ((unsigned int)var6);
            var2 = var7.structVar0.structVar1;
            return var2;
        }
    }
}


