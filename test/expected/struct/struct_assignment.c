
// struct declarations
struct s_mystruct;
union u1;
union u2;
union u3;

// anonymous struct declarations

// type definitions

// struct definitions
struct s_mystruct {
    unsigned int structVar0;
    unsigned char structVar1;
};

// union definitions
union u1 {
    struct s_mystruct* ty0;
    unsigned char* ty1;
};
union u2 {
    struct s_mystruct* ty0;
    unsigned long* ty1;
};
union u3 {
    struct s_mystruct* ty0;
    unsigned long* ty1;
};

// function declarations
unsigned long get_struct(void);
int main(int var0, char** var1);

// global variable definitions
struct s_mystruct __const_get_struct_result = {0,97,};
static unsigned int counter = 0;

unsigned long get_struct(void){
    struct s_mystruct var0;
    union u1 var1;
    union u2 var2;
    block0: ;
    (var1.ty0) = (&var0);
    var0 = __const_get_struct_result;
    counter = (((int)counter) + ((int)1));
    (var2.ty0) = (&var0);
    return *(var2.ty1);
}

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    struct s_mystruct var5;
    union u3 var6;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    (var6.ty0) = (&var5);
    (*(var6.ty1)) = get_struct();
    return ((int)counter) + ((int)(var5.structVar1));
}


