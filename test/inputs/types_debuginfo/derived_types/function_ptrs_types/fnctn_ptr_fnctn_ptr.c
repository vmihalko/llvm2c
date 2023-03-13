// declare the function that takes a function pointer as an argument
void myFunc(int (*funcPtr)(int)) {
    // call the function pointer with an integer argument
    funcPtr(42);
}

// declare a function pointer type that takes an integer argument
typedef int (*funcPtrType)(int);

// define a function that can be passed as a function pointer argument
int printInt(int num) {
    return num;
}

int main() {
    // create a function pointer for the myFunc function
    void (*myFuncPtr)(funcPtrType) = myFunc;

    // call myFuncPtr with the printInt function pointer as an argument
    myFuncPtr(printInt);

    return 0;
}
