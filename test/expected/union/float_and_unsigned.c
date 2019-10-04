
// struct declarations
struct u_float_shape;
union u1;

// anonymous struct declarations

// type definitions

// struct definitions
struct u_float_shape {
    float structVar0;
};

// union definitions
union u1 {
    float* ty0;
    struct u_float_shape* ty1;
    unsigned int* ty2;
};

// function declarations
int main(int var0, char** var1);
extern unsigned int printf(unsigned char* var0, ...);

// global variable definitions
unsigned char _str[4] = {37,117,10,0,};

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    float a;
    struct u_float_shape fs;
    union u1 var7;
    union u1 var8;
    union u1 var9;
    block0: ;
    var2 = 0;
    argc = var0;
    argv = var1;
    a = ((float)argc);
    (var7.ty1) = (&fs);
    (*(var7.ty0)) = a;
    (var8.ty1) = (&fs);
    printf(&(_str[0]), *(var8.ty2));
    (var9.ty1) = (&fs);
    return *(var9.ty2);
}


