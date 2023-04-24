// function declarations
int main(void);

int main(void){
    unsigned int var0;
    int a;
    int* array_of_ptr[1][2];
    unsigned int** var3;
    block0:
    var0 = 0;
    a = 5;
    var3 = (&((array_of_ptr[0])[0]));
    (*var3) = (&a);
    (*(((unsigned int**)(var3)) + 1)) = (&a);
    return ((int)(*((array_of_ptr[0])[1]))) - ((int)5);
}

