// function declarations
unsigned long fact(unsigned long var0);
int main(int argc, char** argv);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

unsigned long fact(unsigned long var0){
    unsigned long num;
    unsigned long result;
    unsigned long l;
    block0:
    num = var0;
    result = 1;
    l = 1;
    goto block1;
    block1:
    if (((long)l) <= ((long)num)) {
        result = (result * l);
        l = (((long)l) + ((long)1));
        goto block1;
    } else {
        return result;
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

