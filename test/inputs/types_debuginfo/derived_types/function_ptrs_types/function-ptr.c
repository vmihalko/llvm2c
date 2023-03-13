int (*func_ptr)(int, int);


int add(int a, int b) {
    return a + b;
}

int main(void) {
	func_ptr = &add;
	return (*func_ptr)(2,3);
}
