
typedef union {
    float f;
    unsigned w;
} float_shape;


int main(int argc, char *argv[])
{
    float a = argc;

    float_shape fs;
    fs.f = a;

    printf("%u\n", fs.w);

    return fs.w;
}
