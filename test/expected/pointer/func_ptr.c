
// struct declarations

// anonymous struct declarations

// type definitions
typedef unsigned long(* typeDef_0)(unsigned long);

// struct definitions

// anonymous struct definitions

// global variable definitions

// function declarations
int main(int var0, char** var1);
unsigned long square(unsigned long var0);
unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    unsigned char* var5;
    unsigned long var6;
    typeDef_0 var7;
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
            var7 = square;
            var2 = ((unsigned int)var7(var6));
            return var2;
        }
    }
}

unsigned long square(unsigned long var0){
    unsigned long var1;
    block0: ;
    var1 = var0;
    return ((long)var1) * ((long)var1);
}


