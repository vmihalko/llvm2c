// struct declarations
struct s_test;
struct s_sptr;

// struct definitions
struct s_test {
    unsigned int structVar0;
};
struct s_sptr {
    struct s_test* structVar1;
};

// function declarations
int main(int argc, char** argv);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

int main(int argc, char** argv){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
    struct s_test t;
    struct s_sptr s;
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
            (t.structVar0) = ((unsigned int)num);
            (s.structVar1) = (&t);
            var2 = ((s.structVar1)->structVar0);
            return var2;
        }
    }
}

