// function declarations
int main(void);

int main(void){
    unsigned int a;
    block0:
    a = 5;
    if (a != 0) {
        goto block2;
    } else {
        a = 3;
        return a;
    }
    block2:
    if (a != 0) {
        a = (((int)a) + ((int)-1));
        goto block2;
    } else {
        return a;
    }
}

