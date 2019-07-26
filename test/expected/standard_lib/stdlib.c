
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// anonymous struct definitions

// function declarations
void free(unsigned char* var0);
int main();
unsigned char* malloc(unsigned long var0);
unsigned int printf(unsigned char* var0, ...);
unsigned char* realloc(unsigned char* var0, unsigned long var1);
unsigned char* strcat(unsigned char* var0, unsigned char* var1);
unsigned char* strcpy(unsigned char* var0, unsigned char* var1);

// global variable definitions
unsigned char _str[11] = {109,97,108,108,111,99,116,101,115,116,0,};
unsigned char _str_1[4] = {37,115,10,0,};
unsigned char _str_2[13] = {32,114,101,97,108,108,111,99,116,101,115,116,0,};
unsigned char _str_3[3] = {37,115,0,};

int main(){
    unsigned int var0;
    unsigned char* var1;
    block0: ;
    var0 = 0;
    var1 = malloc(20);
    strcpy(var1, &_str[0]);
    printf(&_str_1[0], var1);
    var1 = realloc(var1, 40);
    strcat(var1, &_str_2[0]);
    printf(&_str_3[0], var1);
    free(var1);
    return 0;
}


