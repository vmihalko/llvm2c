// struct declarations
union u1;

// union definitions
union u1 {
    unsigned char (*ty0)[20];
    unsigned char* ty1;
};

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
    union u1 var3;
    unsigned char* var4;
    unsigned char* var5;
    block0:
    var0 = 0;
    (var3.ty0) = (&test1);
    memcpy(var3.ty1, &(__const_main_test1[0]), 20);
    var4 = (&(test2[0]));
    var5 = (&(test1[0]));
    memcpy(var4, var5, strlen(&(test1[0])) + 1);
    printf(&(_str[0]), &(test2[0]));
    return 0;
}

