
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
    unsigned int* ty0;
    struct u_float_shape* ty1;
    float* ty2;
};

// function declarations
int main(int var0, char** var1);
extern unsigned int printf(unsigned char* var0, ...);

// global variable definitions
unsigned char _str[4] = {37,117,10,0,};

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    float var5;
    struct u_float_shape var6;
    union u1 var7;
    union u1 var8;
    union u1 var9;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    var5 = ((float)var3);
    (var7.ty1) = (&var6);
    (*(var7.ty2)) = var5;
    (var8.ty1) = (&var6);
    printf(&(_str[0]), *(var8.ty0));
    (var9.ty1) = (&var6);
    return *(var9.ty0);
}


