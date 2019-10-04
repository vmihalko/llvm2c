
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// union definitions

// function declarations
int main(void);

// global variable definitions

int main(void){
    unsigned int var0;
    unsigned long var1;
    block0: ;
    var0 = 0;
    __asm__("mov    $42, %%rbx;   \n\tmov    %%rbx, %0;    \n\t"
        : "=c" (var1)
        : 
        : "%rbx"
    );
    return (unsigned int)var1;
}


