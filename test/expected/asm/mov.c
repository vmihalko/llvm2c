// function declarations
int main(void);

int main(void){
    unsigned long result;
    block0:
    __asm__("mov    $42, %%rbx;   \n\tmov    %%rbx, %0;    \n\t"
        : "=c" (result)
        : 
        : "%rbx"
    );
    return (unsigned int)result;
}

