
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// anonymous struct definitions

// global variable definitions

// function declarations
unsigned int isPrime(unsigned long var0);
int main(int var0, char** var1);
unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

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
        if (((unsigned int)(*var5)) != 0) {
            var2 = -1;
            return var2;
        } else {
            var2 = isPrime(var6);
            return var2;
        }
    }
}

unsigned int isPrime(unsigned long var0){
    unsigned int var1;
    unsigned long var2;
    unsigned int var3;
    block0: ;
    var2 = var0;
    if (((long)var2) < ((long)2)) {
        var1 = 0;
        return var1;
    } else {
        var3 = 2;
        goto block3;
    }
    block3: ;
    if (((long)var3) < ((long)(var2 / 2))) {
        if ((var2 % ((long)var3)) == 0) {
            var1 = 0;
            return var1;
        } else {
            var3 = (((int)var3) + ((int)1));
goto block3;
        }
    } else {
        var1 = 1;
        return var1;
    }
}


