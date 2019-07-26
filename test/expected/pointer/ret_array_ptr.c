
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// anonymous struct definitions

// function declarations
unsigned char (*get_me())[5];
int main(int var0, char** var1);
unsigned char* malloc(unsigned long var0);

// global variable definitions

unsigned char (*get_me())[5]{
    unsigned char* var0;
    block0: ;
    var0 = malloc(5);
    return (unsigned char (*)[5])var0;
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    unsigned char* var5;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    var5 = ((unsigned char*)get_me());
    return (unsigned int)var5;
}


