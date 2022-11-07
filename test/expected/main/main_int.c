// function declarations
int main(int argc, char** argv);
extern unsigned int printf(unsigned char* var0, ...);

// global variable definitions
unsigned char _str[14] = {72,101,108,108,111,32,119,111,114,108,100,33,10,0,};

int main(int argc, char** argv){
    block0:
    printf(&(_str[0]));
    return 0;
}

