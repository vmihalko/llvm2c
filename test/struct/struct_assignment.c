static int counter = 0;

struct mystruct {
    int a;
    char b;
};

struct mystruct get_struct() {
    struct mystruct result = {0, 'a'};
    ++counter;
    return result;
}

int main(int argc, char *argv[])
{
    struct mystruct a = get_struct();
    return counter + a.b;
}
