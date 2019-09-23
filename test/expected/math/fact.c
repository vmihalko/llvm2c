
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// function declarations
unsigned long fact(unsigned long var0);
int main(int var0, char** var1);
unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions

unsigned long fact(unsigned long var0){
    unsigned long var1;
    unsigned long var2;
    unsigned long var3;
    block0: ;
    var1 = var0;
    var2 = 1;
    var3 = 1;
    goto block1;
    block1: ;
    if (((long)var3) <= ((long)var1)) {
        var2 = (var2 * var3);
        var3 = (((long)var3) + ((long)1));
        goto block1;
    } else {
        return var2;
    }
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    unsigned char* var5;
    unsigned long var6;
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
            var2 = ((unsigned int)fact(var6));
            return var2;
        }
    }
}


