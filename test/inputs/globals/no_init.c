
struct some_struct {
    long a;
    long b;
};

struct parent {
    struct some_struct that;
    long c;
    long d;
};

struct parent p;

int main(int argc, char *argv[])
{
    return p.that.b;
}
