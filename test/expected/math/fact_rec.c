
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// union definitions

// function declarations
unsigned long fact(unsigned long var0);
int main(int var0, char** var1);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions

unsigned long fact(unsigned long var0){
    unsigned long var1;
    unsigned long num;
    unsigned long var3;
    block0: ;
    num = var0;
    if (((long)num) > ((long)0)) {
        var3 = num;
        var1 = (var3 * fact(((long)num) - ((long)1)));
        return var1;
    } else {
        var1 = 1;
        return var1;
    }
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
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
            var2 = ((unsigned int)fact(num));
            return var2;
        }
    }
}


