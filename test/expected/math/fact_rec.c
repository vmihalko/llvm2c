// function declarations
unsigned long fact(unsigned long var0);
int main(int argc, char** argv);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

unsigned long fact(unsigned long var0){
    unsigned long var1;
    unsigned long num;
    unsigned long var3;
    block0:
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

int main(int argc, char** argv){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
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
            var2 = ((unsigned int)fact(num));
            return var2;
        }
    }
}

