
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
    unsigned long num;
    unsigned int i;
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
            i = ((unsigned int)num);
            i = (i | 123);
            i = (i & 111);
            var2 = i;
            return var2;
        }
    }
}


