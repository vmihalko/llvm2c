int main() {
	int a = 5;
	int *array_of_ptr[1][2] = {{&a, &a}};
	return (*array_of_ptr[0][1]) - 5;
}
