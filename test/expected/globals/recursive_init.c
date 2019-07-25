
// struct declarations
struct s_list;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_list {
    unsigned long structVar0;
    struct s_list* structVar1;
};

// anonymous struct definitions

// global variable definitions
struct s_list mylist = {0,&mylist,};

// function declarations
int main(int var0, char** var1);

int main(int var0, char** var1){
    block0: ;
    return (unsigned int)mylist.structVar0;
}


