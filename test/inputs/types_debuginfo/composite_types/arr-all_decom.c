// function declarations
int main(void);

// global variable definitions
unsigned int global_arrayl[5] = {1,2,3,4,5,};
unsigned int global_array[3][5] = {{1,2,3,0,0,},{4,5,6,0,0,},{7,8,9,0,0,},};
unsigned int __const_main_local_array[3] = {10,20,30,};
unsigned int __const_main_local_arlray[3][5] = {{10,11,12,0,0,},{13,14,15,0,0,},{16,17,18,0,0,},};

int main(void){
    unsigned int var0;
    unsigned int local_array[3];
    unsigned int local_arlray[3][5];
    block0:
    var0 = 0;
    local_array = __const_main_local_array;
    local_arlray = __const_main_local_arlray;
    return 0;
}

