
typedef struct list {
    long value;
    struct list* next;
} list_t;

list_t mylist = { 0, &mylist };

int main(int argc, char *argv[])
{
    return mylist.value;
}
