// function declarations
unsigned char (*get_me(void))[5];
int main(int argc, char** argv);
extern unsigned char* malloc(unsigned long var0);

unsigned char (*get_me(void))[5]{
    unsigned char* me;
    block0:
    me = malloc(5);
    return (unsigned char (*)[5])me;
}

int main(int argc, char** argv){
    unsigned char* s;
    block0:
    s = ((unsigned char*)get_me());
    return (unsigned int)s;
}

