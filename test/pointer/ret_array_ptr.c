typedef char my_type[5];

my_type* get_me() {
    char *me = malloc(5);
    return me;
}

int main(int argc, char *argv[])
{
    char *s = { get_me() };
    return s;
}
