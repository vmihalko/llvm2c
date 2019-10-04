
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// union definitions

// function declarations
int main(void);
unsigned int printf(unsigned char* var0, ...);

// global variable definitions
unsigned char __const_main_test1[20] = {109,101,109,99,112,121,116,101,115,116,0,0,0,0,0,0,0,0,0,0,};
unsigned char _str[4] = {37,115,10,0,};

int main(void){
    unsigned int var0;
    unsigned char var1[20];
    unsigned char var2[20];
    block0: ;
    var0 = 0;
    memcpy((unsigned char*)(&var1), &(__const_main_test1[0]), 20);
    memcpy(&(var2[0]), &(var1[0]), 11);
    printf(&(_str[0]), &(var2[0]));
    return 0;
}


