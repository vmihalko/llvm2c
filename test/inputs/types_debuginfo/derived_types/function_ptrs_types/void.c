void (*voidInt)(int);

void foo(int a){
	a++;
}

int main(void) {
	voidInt = &foo;
}
