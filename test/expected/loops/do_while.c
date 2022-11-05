// function declarations
int main(int argc, char** argv);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

int main(int argc, char** argv){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
    unsigned int i;
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
            i = 0;
            goto block5;
        }
    }
    block5:
    num = (((long)num) + ((long)((long)((int)i))));
    i = (((int)i) + ((int)1));
    if (((int)i) < ((int)10)) {
        goto block5;
    } else {
        var2 = ((unsigned int)num);
        return var2;
    }
}

