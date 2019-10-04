// function declarations
unsigned int isPrime(unsigned long var0);
int main(int var0, char** var1);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

unsigned int isPrime(unsigned long var0){
    unsigned int var1;
    unsigned long num;
    unsigned int i;
    block0:
    num = var0;
    if (((long)num) < ((long)2)) {
        var1 = 0;
        return var1;
    } else {
        i = 2;
        goto block3;
    }
    block3:
    if (((long)i) < ((long)(num / 2))) {
        if ((num % ((long)i)) == 0) {
            var1 = 0;
            return var1;
        } else {
            i = (((int)i) + ((int)1));
            goto block3;
        }
    } else {
        var1 = 1;
        return var1;
    }
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned char* p;
    unsigned long num;
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
            var2 = isPrime(num);
            return var2;
        }
    }
}

