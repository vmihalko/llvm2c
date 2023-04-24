// struct declarations
struct s_s_mystruct;

// type definitions
typedef unsigned int(* typeDef_0)(unsigned int);

// struct definitions
struct s_s_mystruct {
    typeDef_0 structVar0;
};

// function declarations
int main(int argc, char** argv);
unsigned int square(unsigned int var0);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions
struct s_s_mystruct test = {square,};

int main(int argc, char** argv){
    unsigned int var7;
    unsigned int argc;
    unsigned char** argv;
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    unsigned char* var5;
    unsigned long var6;
    block0:
    var7 = 0;
    argc = argc;
    argv = argv;
    var2 = 0;
    var3 = argc;
    var4 = argv;
    if (var3 != 2) {
        var2 = -1;
        var7 = var2;
        return var7;
    } else {
        var6 = strtol(*(((unsigned char**)(var4)) + 1), &var5, 10);
        if (((int)((char)(*var5))) != 0) {
            var2 = -1;
            var7 = var2;
            return var7;
        } else {
            var2 = (test.structVar0)((unsigned int)var6);
            var7 = var2;
            return var7;
        }
    }
}

unsigned int square(unsigned int var0){
    unsigned int var0;
    unsigned int var1;
    block0:
    var0 = var0;
    var1 = var0;
    return ((int)var1) * ((int)var1);
}

