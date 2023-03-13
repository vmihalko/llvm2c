// type definitions
typedef void(* typeDef_0)(unsigned int);

// function declarations
void foo(unsigned int var0);
int main(void);

// global variable definitions
typeDef_0 voidInt = 0;

void foo(unsigned int var0){
    unsigned int a;
    block0:
    a = var0;
    a = (((int)a) + ((int)1));
    return;
}

int main(void){
    block0:
    voidInt = foo;
    return 0;
}

