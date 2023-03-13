// Pointer types
int *ip;
void *vp2;
char *cp;
int (*func_ptr)(int, int);


int add(int a, int b) {
    return a + b;
}

int main(void) {
    //mycolor = GREEN;
    ip = 0x0;
    cp = "hello";
    vp2 = 0x0;
    func_ptr = &add;
    return 0;
}
