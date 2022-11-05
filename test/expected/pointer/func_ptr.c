// type definitions
typedef unsigned long(* typeDef_0)(unsigned long);

// function declarations
int main(int argc, char** argv);
unsigned long square(unsigned long var0);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

int main(int argc, char** argv){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
    typeDef_0 ptr;
    block0:
    var2 = 0;
    argc = argc;
    argv = argv;
    if (argc != 2) {
        var2 = -1;
        return var2;
    } else {
        num = strtol(*(((unsigned char**)(argv)) + 1), &p, 10);
        if (((int)((char)(*p))) != 0) {
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
    block0:
    l = var0;
    return ((long)l) * ((long)l);
}

