
// struct declarations
struct u_float_shape;
union u1;
union u2;
union u3;

// anonymous struct declarations

// type definitions

// struct definitions
struct u_float_shape {
    float structVar0;
};

// union definitions
union u1 {
    struct u_float_shape* ty0;
    float* ty1;
};
union u2 {
    struct u_float_shape* ty0;
    unsigned int* ty1;
};
union u3 {
    struct u_float_shape* ty0;
    unsigned int* ty1;
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
    union u2 var8;
    union u3 var9;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    var5 = ((float)var3);
    (var7.ty0) = (&var6);
    (*(var7.ty1)) = var5;
    (var8.ty0) = (&var6);
    printf(&(_str[0]), *(var8.ty1));
    (var9.ty0) = (&var6);
    return *(var9.ty1);
}


