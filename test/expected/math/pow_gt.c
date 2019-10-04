
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// union definitions

// function declarations
int main(int var0, char** var1);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long l;
    unsigned long r;
    block0: ;
    var2 = 0;
    argc = var0;
    argv = var1;
    if (argc != 3) {
        var2 = -1;
        return var2;
    } else {
        l = strtol(*(((unsigned char**)(argv)) + 1), &p, 10);
        if (((int)(*p)) != 0) {
            var2 = -1;
            return var2;
        } else {
            r = strtol(*(((unsigned char**)(argv)) + 2), &p, 10);
            if (((int)(*p)) != 0) {
                var2 = -1;
                return var2;
            } else {
                if (((long)(l * l)) > ((long)(r * r))) {
                    var2 = ((unsigned int)l);
                    return var2;
                } else {
                    var2 = ((unsigned int)r);
                    return var2;
                }
            }
        }
    }
}


