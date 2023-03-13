// struct declarations
struct s_parent;
struct s_some_struct;

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

// function declarations
int main(int argc, char** argv);

// global variable definitions
struct s_parent p = {{0,0},0,0};

int main(int argc, char** argv){
    block0:
    return (unsigned int)((p.structVar0).structVar4);
}

