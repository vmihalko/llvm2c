
// struct declarations
struct s_mystruct;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_mystruct {
    unsigned int structVar0;
    unsigned char structVar1;
};

// function declarations
unsigned long get_struct();
int main(int var0, char** var1);

// global variable definitions
struct s_mystruct __const_get_struct_result = {0,97,};
static unsigned int counter = 0;

unsigned long get_struct(){
    struct s_mystruct var0;
    block0: ;
    var0 = __const_get_struct_result;
    counter = (((int)counter) + ((int)1));
    return *((unsigned long*)(&var0));
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    struct s_mystruct var5;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    (*((unsigned long*)(&var5))) = get_struct();
    return ((int)counter) + ((int)var5.structVar1);
}


