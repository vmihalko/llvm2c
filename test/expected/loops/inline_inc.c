// function declarations
int main(int argc, char** argv);

int main(int argc, char** argv){
    unsigned int var2;
    unsigned int argc;
    unsigned char** argv;
    unsigned int i;
    int var6;
    block0:
    var2 = 0;
    argc = argc;
    argv = argv;
    i = 0;
    goto block1;
    block1:
    var6 = (((int)i) + ((int)1));
    i = var6;
    if (var6 < ((int)10)) {
        goto block1;
    } else {
        return i;
    }
}

