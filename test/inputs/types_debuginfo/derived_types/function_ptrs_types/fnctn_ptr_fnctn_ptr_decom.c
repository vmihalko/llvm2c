// type definitions
typedef int(* typeDef_0)(int);
typedef void(* typeDef_1)(typeDef_0);

// function declarations
int main(void);
void myFunc(typeDef_0 var0);
unsigned int printInt(unsigned int var0);

int main(void){
    unsigned int var0;
    typeDef_1 myFuncPtr;
    block0:
    var0 = 0;
    myFuncPtr = myFunc;
    myFuncPtr(printInt);
    return 0;
}

void myFunc(typeDef_0 var0){
    typeDef_0 funcPtr;
    block0:
    funcPtr = var0;
    funcPtr(42);
    return;
}

unsigned int printInt(unsigned int var0){
    int num;
    block0:
    num = var0;
    return num;
}

