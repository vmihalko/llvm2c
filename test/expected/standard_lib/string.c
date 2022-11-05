// function declarations
int main(void);
extern unsigned int printf(unsigned char* var0, ...);
extern unsigned long strlen(unsigned char* var0);

// global variable definitions
unsigned char __const_main_test1[20] = {109,101,109,99,112,121,116,101,115,116,0,0,0,0,0,0,0,0,0,0,};
unsigned char _str[4] = {37,115,10,0,};

int main(void){
    unsigned int var0;
    unsigned char test1[20];
    unsigned char test2[20];
    unsigned char* var3;
    unsigned char* var4;
    block0:
    var0 = 0;
    memcpy((unsigned char*)(&test1), &(__const_main_test1[0]), 20);
    var3 = (&(test2[0]));
    var4 = (&(test1[0]));
    memcpy(var3, var4, strlen(&(test1[0])) + 1);
    printf(&(_str[0]), &(test2[0]));
    return 0;
}

