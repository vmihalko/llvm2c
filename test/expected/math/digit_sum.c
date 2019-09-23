
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// function declarations
unsigned int digitSum(unsigned long var0);
int main(int var0, char** var1);
unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions

unsigned int digitSum(unsigned long var0){
    unsigned long var1;
    unsigned long var2;
    unsigned int var3;
    unsigned int var4;
    block0: ;
    var1 = var0;
    var2 = var1;
    var3 = 0;
    var4 = 0;
    goto block1;
    block1: ;
    if (var2 != 0) {
        var3 = ((unsigned int)(((long)var2) % ((long)10)));
        var4 = (((int)var4) + ((int)var3));
        var2 = (((long)var2) / ((long)10));
        goto block1;
    } else {
        return var4;
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
            var2 = digitSum(var6);
            return var2;
        }
    }
}


