
// struct declarations
struct s_mystruct;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_mystruct {
    unsigned int structVar0;
    unsigned int structVar1;
    unsigned int structVar2;
};

// anonymous struct definitions

// global variable definitions
struct s_mystruct i = {0,0,0,};

// function declarations
struct s_mystruct* get_mystruct();
int main(int var0, char** var1);

struct s_mystruct* get_mystruct(){
    block0: ;
    return &i;
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    struct s_mystruct* var5;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    var5 = get_mystruct();
    return 0;
}


