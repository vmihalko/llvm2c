// struct declarations
struct s_mystruct;

// type definitions
typedef unsigned int(* typeDef_0)(unsigned int);

// struct definitions
struct s_mystruct {
    typeDef_0 structVar0;
};

// function declarations
int main(int var0, char** var1);
unsigned int square(unsigned int var0);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions
struct s_mystruct test = {square,};

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
    block0:
    var2 = 0;
    argc = var0;
    argv = var1;
    if (argc != 2) {
        var2 = -1;
        return var2;
    } else {
        num = strtol(*(((unsigned char**)(argv)) + 1), &p, 10);
        if (((int)(*p)) != 0) {
            var2 = -1;
            return var2;
        } else {
            var2 = (test.structVar0)((unsigned int)num);
            return var2;
        }
    }
}

unsigned int square(unsigned int var0){
    unsigned int x;
    block0:
    x = var0;
    return ((int)x) * ((int)x);
}

