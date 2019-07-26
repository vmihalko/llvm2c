
int square(int x) {
    return x * x;
}

struct mystruct {
    int (*myfunc)(int a);
};

struct mystruct test = { square };

int main(int argc, char *argv[])
{
    if (argc != 2) {
        return -1;
    }

	char *p;
	long num = strtol(argv[1], &p, 10);

	if (*p != '\0') {
		return -1;
	} else {
		return test.myfunc(num);
	}
}
