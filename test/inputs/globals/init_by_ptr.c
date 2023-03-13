struct mystruct {
    int *ptr_to_a;
    int a;
};

struct mystruct test = { &test.a, 5 };

int main(void)
{
	return *(test.ptr_to_a);
}
