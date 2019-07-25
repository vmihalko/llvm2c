
// struct declarations

// anonymous struct declarations

// type definitions

// struct definitions

// anonymous struct definitions

// global variable definitions

// function declarations
unsigned long strtol(unsigned char* var0, unsigned char** var1, unsigned int var2);
int main(int var0, char** var1);

int main(int var0, char** var1){
    unsigned int var2;
    unsigned int var3;
    unsigned char** var4;
    unsigned char* var5;
    unsigned long var6;
    unsigned long var7;
    unsigned int var8;
    block0: ;
    var2 = 0;
    var3 = var0;
    var4 = var1;
    if (var3 != 3) {
        var2 = -1;
        return var2;
    } else {
        var6 = strtol(*(((unsigned char**)(var4)) + 1), &var5, 10);
        if (((unsigned int)(*var5)) != 0) {
            var2 = -1;
            return var2;
        } else {
            var7 = strtol(*(((unsigned char**)(var4)) + 2), &var5, 10);
            if (((unsigned int)(*var5)) != 0) {
                var2 = -1;
                return var2;
            } else {
                __asm__("addl %%ebx, %%eax;"
        : "=a" (var8)
        : "b" (var7), "a" (var6)
        : 
    );;
                __asm__("subl %%ebx, %%eax;"
        : "=a" (var8)
        : "b" (10), "a" (var8)
        : 
    );;
                __asm__("imull %%ebx, %%eax;"
        : "=a" (var8)
        : "b" (var7), "a" (var8)
        : 
    );;
                var2 = var8;
                return var2;
            }
        }
    }
}


