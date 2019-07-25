struct mystruct {
    int a;
    int b;
    int c;
};

struct mystruct i = {0,0,0};

struct mystruct* get_mystruct() {
    return &i;
}

int main(int argc, char *argv[])
{
    struct mystruct* a = get_mystruct();
    return 0;
}
