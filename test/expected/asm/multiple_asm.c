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
    unsigned int result;
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
                __asm__("addl %%ebx, %%eax;"
                    : "=a" (result)
                    : "b" (r), "a" (l)
                    : 
                );
                __asm__("subl %%ebx, %%eax;"
                    : "=a" (result)
                    : "b" (10), "a" (result)
                    : 
                );
                __asm__("imull %%ebx, %%eax;"
                    : "=a" (result)
                    : "b" (r), "a" (result)
                    : 
                );
                var2 = result;
                return var2;
            }
        }
    }
}

