// function declarations
int main(void);

// global variable definitions
unsigned char __const_main_source[7] = {110,111,105,110,105,116,0,};
unsigned char dest[7] = {0,0,0,0,0,0,0,};

int main(void){
    unsigned char source[7];
    block0:
    memcpy((unsigned char*)(&source), &(__const_main_source[0]), 7);
    memcpy(&(dest[0]), &(source[0]), 7);
    return 0;
}

