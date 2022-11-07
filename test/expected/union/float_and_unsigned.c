// struct declarations
struct u_float_shape;

// struct definitions
struct u_float_shape {
    float structVar0;
};

// function declarations
int main(int argc, char** argv);
extern unsigned int printf(unsigned char* var0, ...);

// global variable definitions
unsigned char _str[4] = {37,117,10,0,};

int main(int argc, char** argv){
    unsigned int argc;
    float a;
    struct u_float_shape fs;
    block0:
    argc = argc;
    a = ((float)argc);
    (*((float*)(&fs))) = a;
    printf(&(_str[0]), *((unsigned int*)(&fs)));
    return *((unsigned int*)(&fs));
}

