// function declarations
int main(int argc, char** argv);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

int main(int argc, char** argv){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long l;
    unsigned long r;
    block0:
    var2 = 0;
    argc = argc;
    argv = argv;
    if (argc != 3) {
        var2 = -1;
        return var2;
    } else {
        l = strtol(*(((unsigned char**)(argv)) + 1), &p, 10);
        if (((int)((char)(*p))) != 0) {
            var2 = -1;
            return var2;
        } else {
            r = strtol(*(((unsigned char**)(argv)) + 2), &p, 10);
            if (((int)((char)(*p))) != 0) {
                var2 = -1;
                return var2;
            } else {
                if (l != 0) {
                    var2 = ((unsigned int)(((long)r) % ((long)l)));
                    return var2;
                } else {
                    var2 = ((unsigned int)(((long)l) % ((long)r)));
                    return var2;
                }
            }
        }
    }
}

