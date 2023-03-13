// type definitions
typedef int(* typeDef_1)(int, int);

// function declarations
unsigned int add(unsigned int var0, unsigned int var1);
int main(void);

// global variable definitions
typeDef_1 func_ptr = 0;

unsigned int add(unsigned int var0, unsigned int var1){
    int a;
    int b;
    block0:
    a = var0;
    b = var1;
    return ((int)a) + ((int)b);
}

int main(void){
    unsigned int var0;
    block0:
    var0 = 0;
    func_ptr = add;
    return func_ptr(2, 3);
}

