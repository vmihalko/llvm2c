#include <stdio.h>

// global array
int global_arrayl[5] = {1, 2, 3, 4, 5};
void modify_array(int arr[][10], int rows);
int global_array[3][5] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
};
void print_array(int arr[], int size);
int main() {
    // local array in main function
    int local_array[3] = {10, 20, 30};

    int local_arlray[3][5] = {
        {10, 11, 12},
        {13, 14, 15},
        {16, 17, 18}
    };
    return 0;
}
