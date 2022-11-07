// function declarations
int main(int argc, char** argv);

int main(int argc, char** argv){
    unsigned int i;
    int var6;
    block0:
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

