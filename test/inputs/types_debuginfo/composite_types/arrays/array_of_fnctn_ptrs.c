int fun() {
	return 0;
}

int main() {
	int (*function_array[3])() = {&fun, &fun, &fun};
	return (*function_array[2])();
}
