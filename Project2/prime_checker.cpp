#include <stdio.h>
#include <stdlib.h>
bool isPrime(int num) {
    if (num <= 1) {
        return false;
    }
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <start_num> <end_num>\n", argv[0]);
        return 1;
    }

    int start = atoi(argv[1]);
    int end = atoi(argv[2]);

    for (int num = start; num <= end; ++num) {
        if (isPrime(num)) {
            printf("%d ", num);
        }
    }

    return 0;
}