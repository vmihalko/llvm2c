// struct declarations
struct s_test;

// struct definitions
struct s_test {
    unsigned int structVar0;
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
    struct s_test arr[5];
    unsigned int i;
    unsigned int sum;
    unsigned int i2;
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
            i = 0;
            goto block5;
        }
    }
    block5:
    if (((int)i) < ((int)5)) {
        ((arr[(long)i]).structVar0) = ((unsigned int)(((long)num) + ((long)i)));
        i = (((int)i) + ((int)1));
        goto block5;
    } else {
        sum = 0;
        i2 = 0;
        goto block9;
    }
    block9:
    if (((int)i2) < ((int)5)) {
        sum = (((int)sum) + ((int)((arr[(long)i2]).structVar0)));
        i2 = (((int)i2) + ((int)1));
        goto block9;
    } else {
        var2 = sum;
        return var2;
    }
}

