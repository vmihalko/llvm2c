// function declarations
unsigned int digitSum(unsigned long var0);
int main(int argc, char** argv);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

unsigned int digitSum(unsigned long var0){
    unsigned long num;
    unsigned long l;
    unsigned int rem;
    unsigned int sum;
    block0:
    num = var0;
    l = num;
    rem = 0;
    sum = 0;
    goto block1;
    block1:
    if (l != 0) {
        rem = ((unsigned int)(((long)l) % ((long)10)));
        sum = (((int)sum) + ((int)rem));
        l = (((long)l) / ((long)10));
        goto block1;
    } else {
        return sum;
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
            var2 = digitSum(num);
            return var2;
        }
    }
}

