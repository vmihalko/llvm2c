
// struct declarations
struct s_test;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_test {
    unsigned int structVar0;
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
    struct s_test var7[5];
    unsigned int var8;
    unsigned int var9;
    unsigned int var10;
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
            var8 = 0;
            goto block5;
        }
    }
    block5: ;
    if (((int)var8) < ((int)5)) {
        var7[(long)var8].structVar0 = ((unsigned int)(((long)var6) + ((long)var8)));
        var8 = (((int)var8) + ((int)1));
goto block5;
    } else {
        var9 = 0;
        var10 = 0;
        goto block9;
    }
    block9: ;
    if (((int)var10) < ((int)5)) {
        var9 = (((int)var9) + ((int)var7[(long)var10].structVar0));
        var10 = (((int)var10) + ((int)1));
goto block9;
    } else {
        var2 = var9;
        return var2;
    }
}


