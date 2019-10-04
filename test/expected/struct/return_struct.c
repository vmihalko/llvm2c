
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

// union definitions

// function declarations
struct s_mystruct* get_mystruct(void);
int main(int var0, char** var1);

// global variable definitions
struct s_mystruct i = {0,0,0,};

struct s_mystruct* get_mystruct(void){
    block0: ;
    return &i;
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    struct s_mystruct* a;
    block0: ;
    var2 = 0;
    argc = var0;
    argv = var1;
    a = get_mystruct();
    return 0;
}


