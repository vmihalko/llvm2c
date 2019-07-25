
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

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
    unsigned int var7;
    unsigned int var8;
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
            var7 = 0;
            goto block5;
        }
    }
    block5: ;
    if (((int)var7) < ((int)10)) {
        var8 = 5;
        goto block7;
    } else {
        var2 = ((unsigned int)var6);
        return var2;
    }
    block7: ;
    if (((int)var8) < ((int)(var7 + 10))) {
        var6 = (((long)(((long)var6) + ((long)var8))) - ((long)var7));
        var8 = (((int)var8) + ((int)1));
goto block7;
    } else {
        var7 = (((int)var7) + ((int)1));
        goto block5;
    }
}


