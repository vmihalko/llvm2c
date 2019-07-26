
// struct declarations
struct s_mystruct;

// anonymous struct declarations

// type definitions
typedef unsigned int(* typeDef_0)(unsigned int);

// struct definitions
struct s_mystruct {
    typeDef_0 structVar0;
};

// function declarations
int main(int var0, char** var1);
unsigned int square(unsigned int var0);
unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions
struct s_mystruct test = {square,};

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
            var2 = test.structVar0((unsigned int)var6);
            return var2;
        }
    }
}

unsigned int square(unsigned int var0){
    unsigned int var1;
    block0: ;
    var1 = var0;
    return ((int)var1) * ((int)var1);
}


