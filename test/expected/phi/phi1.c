// function declarations
int main(int argc, char** argv);
extern unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);

int main(int argc, char** argv){
    unsigned char* p;
    unsigned int var3_phi;
    block0:
    switch ((unsigned int)strtol(*(((unsigned char**)(argv)) + 1), &p, 10)) {
        case 1:
            var3_phi = 43;
            return var3_phi;
        case 2:
            var3_phi = 42;
            return var3_phi;
        case 3:
            var3_phi = 44;
            return var3_phi;
        case 4:
            var3_phi = 30;
            return var3_phi;
        case 5:
            var3_phi = 21;
            return var3_phi;
        case 6:
            var3_phi = 40;
            return var3_phi;
        case 7:
            var3_phi = 0;
            return var3_phi;
        case 8:
            var3_phi = 70;
            return var3_phi;
        case 9:
            var3_phi = 11;
            return var3_phi;
        case 10:
            var3_phi = 1;
            return var3_phi;
        default:
            return var3_phi;
}
}

