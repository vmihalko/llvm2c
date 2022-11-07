// struct declarations
struct s_mystruct;

// struct definitions
struct s_mystruct {
    unsigned int structVar0;
    unsigned int structVar1;
    unsigned int structVar2;
};

// function declarations
struct s_mystruct* get_mystruct(void);
int main(int argc, char** argv);

// global variable definitions
struct s_mystruct i = {0,0,0,};

struct s_mystruct* get_mystruct(void){
    block0:
    return &i;
}

int main(int argc, char** argv){
    block0:
    return 0;
}

