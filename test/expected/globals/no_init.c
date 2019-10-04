
// struct declarations
struct s_parent;
struct s_some_struct;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_some_struct {
    unsigned long structVar3;
    unsigned long structVar4;
};
struct s_parent {
    struct s_some_struct structVar0;
    unsigned long structVar1;
    unsigned long structVar2;
};

// union definitions

// function declarations
int main(int var0, char** var1);

// global variable definitions
struct s_parent p = {{0,0,},0,0,};

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    return (unsigned int)((p.structVar0).structVar4);
}


