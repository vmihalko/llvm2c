
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// union definitions

// function declarations
unsigned int digitSum(unsigned long var0);
int main(int var0, char** var1);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

// global variable definitions

unsigned int digitSum(unsigned long var0){
    unsigned long num;
    unsigned long l;
    unsigned int rem;
    unsigned int sum;
    block0: ;
    num = var0;
    l = num;
    rem = 0;
    sum = 0;
    goto block1;
    block1: ;
    if (l != 0) {
        rem = ((unsigned int)(((long)l) % ((long)10)));
        sum = (((int)sum) + ((int)rem));
        l = (((long)l) / ((long)10));
        goto block1;
    } else {
        return sum;
    }
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
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
            var2 = digitSum(num);
            return var2;
        }
    }
}


