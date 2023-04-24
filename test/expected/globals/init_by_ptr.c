// struct declarations
struct s_mystruct;

// struct definitions
struct s_mystruct {
    unsigned int* structVar0;
    unsigned int structVar1;
};

// function declarations
int main(void);

// global variable definitions
struct s_mystruct test = {(unsigned int*)(&(*(((unsigned char*)((unsigned char*)(&test))) + 8))),5,};

int main(void){
    block0:
    return *(test.structVar0);
}

