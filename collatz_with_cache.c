
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s N MIN MAX\n", argv[0]);
        return 1; 
    }

    int N = atoi(argv[1]);
    int MIN = atoi(argv[2]);
    int MAX = atoi(argv[3]);
    if (N <= 0 || MIN > MAX) {
        printf("Error: Invalid arguments. Ensure N > 0 and MIN <= MAX.\n");
        return 1;
    }

    printf("Number of values to test (N): %d\n", N);
    printf("Smallest value to test (MIN): %d\n", MIN);
    printf("Largest value to test (MAX): %d\n", MAX);

    // Open a file for writing CSV output
    FILE *file = fopen("output.csv", "w");
    if (file == NULL) {
        printf("Error: Unable to open file for writing.\n");
        return 1;
    }

    // Write the header row to the CSV file
    fprintf(file, "RN, Steps, Time\n");

    srand(time(NULL));  // Seed the random number generator once, before the loop

    for (int i = 0; i < N; i++) {
        int initial_RN = (rand() % (MAX - MIN + 1)) + MIN;  // Store the initial random number
        int RN = initial_RN;  // Copy to RN for modification in Collatz process

        printf("Random number (RN) between %d and %d: %d\n", MIN, MAX, initial_RN);
        
        clock_t start_time = clock();
        int steps = 0;
        while (RN != 1) {
            if (RN % 2 == 1) {
                RN = (RN * 3) + 1;
            } else {
                RN = RN / 2;
            }
            printf("%d\n", RN);
            steps++;
        }
        clock_t end_time = clock();
        double time_taken = ((double)(end_time - start_time))/CLOCKS_PER_SEC;
        printf("Reached 1, terminating sequence.\n");
        printf("Collatz steps for %d: %d\n", initial_RN, steps);
        printf("Time: %f seconds\n", time_taken);

        // Write the results (initial RN and steps) to the CSV file
        fprintf(file, "%d,%d,%f\n", initial_RN, steps, time_taken);
    }

    // Close the file
    fclose(file);
    printf("Results have been written to output.csv\n");

    return 0;
}
