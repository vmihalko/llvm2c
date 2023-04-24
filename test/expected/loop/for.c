// function declarations
int main(void);
extern unsigned int putchar(unsigned int var0);

int main(void){
    unsigned int var0;
    unsigned int i;
    block0:
    var0 = 0;
    i = 0;
    goto block1;
    block1:
    if (((int)i) < ((int)10)) {
        putchar(((int)i) + ((int)48));
        i = (((int)i) + ((int)1));
        goto block1;
    } else {
        return var0;
    }
}

