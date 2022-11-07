// function declarations
int main(void);
extern unsigned int printf(unsigned char* var0, ...);

// global variable definitions
unsigned char _str[7] = {37,46,49,108,102,10,0,};

int main(void){
    float f;
    block0:
    f = 1.23E+2;
    f = ((float)log10((double)f));
    f = ((float)pow((double)f, 2.0E+0));
    printf(&(_str[0]), floor((double)f));
    return 0;
}

