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
    block0:
    return (unsigned int)(mylist.structVar0);
}

