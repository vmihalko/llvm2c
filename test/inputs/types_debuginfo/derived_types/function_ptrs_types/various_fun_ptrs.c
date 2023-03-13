int (*func_ptr)(int, int);
int (*intintint)(int, int);
void (*voidint)(int);
void (*voidvoid)(void);
int* (*pintvoid)(void);
int**** (*ppppintvoid)(void);
int (*intppppchar)(char ****);
int* (*pintirray)(int []);



int add(int a, int b) {
    return a + b;
}

int main(void) {
	func_ptr = &add;
	return (*func_ptr)(2,3);
}
