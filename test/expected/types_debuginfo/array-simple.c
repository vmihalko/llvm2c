// function declarations
int main(void);

// global variable definitions
unsigned int __const_main_a[2] = {1,2,};

int main(void){
    unsigned int a[2];
    block0:
    a = __const_main_a;
    return a[0];
}

