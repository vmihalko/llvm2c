
// struct declarations

// anonymous struct declarations

// type definitions
typedef unsigned long(* typeDef_0)(unsigned long);

// struct definitions

// union definitions

// function declarations
int main(int var0, char** var1);
unsigned long square(unsigned long var0);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
    typeDef_0 ptr;
    block0: ;
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
            ptr = square;
            var2 = ((unsigned int)ptr(num));
            return var2;
        }
    }
}

unsigned long square(unsigned long var0){
    unsigned long l;
    block0: ;
    l = var0;
    return ((long)l) * ((long)l);
}


