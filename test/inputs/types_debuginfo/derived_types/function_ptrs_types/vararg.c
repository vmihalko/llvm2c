#include <stdarg.h>

int print_args(int count, ...) {
    int r = 0;
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++) {
        int arg = va_arg(args, int);
	r += arg;
    }
    va_end(args);
    return r;
}

int (*func_ptr)(int, ...) = &print_args;

int main(void) {
	return (*func_ptr)(3, 1,2,3);
}
