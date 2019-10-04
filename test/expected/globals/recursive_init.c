
// struct declarations
struct s_list;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_list {
    unsigned long structVar0;
    struct s_list* structVar1;
};

// union definitions

// function declarations
int main(int var0, char** var1);

// global variable definitions
struct s_list mylist = {0,&mylist,};

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    block0: ;
    var2 = 0;
    argc = var0;
    argv = var1;
    return (unsigned int)(mylist.structVar0);
}


