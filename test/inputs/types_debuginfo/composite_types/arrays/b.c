typedef int(* typeDef_0)(void);

// function declarations
unsigned int fun(void);
int main(void);

// global variable definitions
typeDef_0 __const_main_function_array[3] = {(typeDef_0)fun,(typeDef_0)fun,(typeDef_0)fun,};

unsigned int fun(void){
    block0:
    return 0;
}

int main(void){
    unsigned int var0;
    block0:
    var0 = 0;
    typeDef_0 function_array = __const_main_function_array;
    return (function_array[2])();
}

