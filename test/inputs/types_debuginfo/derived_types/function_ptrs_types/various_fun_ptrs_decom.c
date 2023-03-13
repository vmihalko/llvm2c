// type definitions
typedef int(* typeDef_0)(int, int);
typedef void(* typeDef_1)(int);
typedef void(* typeDef_2)();
typedef int*(* typeDef_3)();
typedef int****(* typeDef_4)();
typedef int(* typeDef_5)(char****);
typedef int*(* typeDef_6)(int*);

// function declarations
unsigned int add(unsigned int var0, unsigned int var1);
int main(void);

// global variable definitions
typeDef_0 func_ptr = 0;
typeDef_0 intintint = 0;
typeDef_1 voidint = 0;
typeDef_2 voidvoid = 0;
typeDef_3 pintvoid = 0;
typeDef_4 ppppintvoid = 0;
typeDef_5 intppppchar = 0;
typeDef_6 pintirray = 0;

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

