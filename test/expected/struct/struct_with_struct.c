// struct declarations
struct s_two;
struct s_one;

// struct definitions
struct s_one {
    unsigned int structVar1;
};
struct s_two {
    struct s_one structVar0;
};

// function declarations
int main(int var0, char** var1);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
    struct s_two test;
    block0:
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
            ((test.structVar0).structVar1) = ((unsigned int)num);
            var2 = ((test.structVar0).structVar1);
            return var2;
        }
    }
}

