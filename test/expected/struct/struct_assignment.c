// struct declarations
struct s_mystruct;

// struct definitions
struct s_mystruct {
    unsigned int structVar0;
    unsigned char structVar1;
};

// function declarations
unsigned long get_struct(void);
int main(int argc, char** argv);

// global variable definitions
struct s_mystruct __const_get_struct_result = {0,97,};
static unsigned int counter = 0;

unsigned long get_struct(void){
    struct s_mystruct result;
    block0:
    result = __const_get_struct_result;
    counter = (((int)counter) + ((int)1));
    return *((unsigned long*)(&result));
}

int main(int argc, char** argv){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    struct s_mystruct a;
    block0:
    var2 = 0;
    argc = argc;
    argv = argv;
    (*((unsigned long*)(&a))) = get_struct();
    return ((int)counter) + ((int)((int)((char)(a.structVar1))));
}

