// struct declarations
struct s_list;

// struct definitions
struct s_list {
    unsigned long structVar0;
    struct s_list* structVar1;
};

// function declarations
int main(int argc, char** argv);

// global variable definitions
struct s_list mylist = {0,&mylist,};

int main(int argc, char** argv){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    block0:
    var2 = 0;
    argc = argc;
    argv = argv;
    return (unsigned int)(mylist.structVar0);
}

