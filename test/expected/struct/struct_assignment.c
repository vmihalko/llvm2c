// struct declarations
struct s_mystruct;
union u1;

// struct definitions
struct s_mystruct {
    unsigned int structVar0;
    unsigned char structVar1;
};

// union definitions
union u1 {
    unsigned char* ty0;
    struct s_mystruct* ty1;
    unsigned long* ty2;
};

// function declarations
unsigned long get_struct(void);
int main(int var0, char** var1);

// global variable definitions
struct s_mystruct __const_get_struct_result = {0,97,};
static unsigned int counter = 0;

unsigned long get_struct(void){
    struct s_mystruct result;
    union u1 var1;
    union u1 var2;
    block0:
    (var1.ty1) = (&result);
    result = __const_get_struct_result;
    counter = (((int)counter) + ((int)1));
    (var2.ty1) = (&result);
    return *(var2.ty2);
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    struct s_mystruct a;
    union u1 var6;
    block0:
    var2 = 0;
    argc = var0;
    argv = var1;
    (var6.ty1) = (&a);
    (*(var6.ty2)) = get_struct();
    return ((int)counter) + ((int)(a.structVar1));
}

