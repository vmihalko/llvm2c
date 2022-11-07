// function declarations
extern void free(unsigned char* var0);
int main(void);
extern unsigned char* malloc(unsigned long var0);
extern unsigned int printf(unsigned char* var0, ...);
extern unsigned char* realloc(unsigned char* var0, unsigned long var1);
extern unsigned char* strcat(unsigned char* var0, unsigned char* var1);
extern unsigned char* strcpy(unsigned char* var0, unsigned char* var1);

// global variable definitions
unsigned char _str[11] = {109,97,108,108,111,99,116,101,115,116,0,};
unsigned char _str_1[4] = {37,115,10,0,};
unsigned char _str_2[13] = {32,114,101,97,108,108,111,99,116,101,115,116,0,};
unsigned char _str_3[3] = {37,115,0,};

int main(void){
    unsigned char* str;
    block0:
    str = malloc(20);
    strcpy(str, &(_str[0]));
    printf(&(_str_1[0]), str);
    str = realloc(str, 40);
    strcat(str, &(_str_2[0]));
    printf(&(_str_3[0]), str);
    free(str);
    return 0;
}

