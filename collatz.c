#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    int number;
    int steps;
    clock_t last_used;
} CacheEntry;

CacheEntry* cache = NULL;
int cache_size = 0;
int current_cache_count = 0;
int cache_hits = 0;
int total_lookups = 0;

int collatz_steps(int number);
int cache_get(int number, int *steps);
void cache_put(int number, int steps, const char *policy);
double calculate_hit_percentage();

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s N MIN MAX cache_policy cache_size\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int MIN = atoi(argv[2]);
    int MAX = atoi(argv[3]);
    const char *cache_policy = argv[4];
    cache_size = atoi(argv[5]);

    if (N <= 0 || MIN > MAX || cache_size <= 0) {
        printf("Error: Invalid arguments. Ensure N > 0, MIN <= MAX, and cache_size > 0.\n");
        return 1;
    }

    printf("Number of values to test (N): %d\n", N);
    printf("Smallest value to test (MIN): %d\n", MIN);
    printf("Largest value to test (MAX): %d\n", MAX);
    printf("Cache policy: %s\n", cache_policy);
    printf("Cache size: %d\n", cache_size);

    cache = (CacheEntry*)malloc(sizeof(CacheEntry) * cache_size);
    if (cache == NULL) {
        printf("Error: Unable to allocate cache memory.\n");
        return 1;
    }

    FILE *file = fopen("output.csv", "w");
    if (file == NULL) {
        printf("Error: Unable to open file for writing.\n");
        free(cache);
        return 1;
    }

    fprintf(file, "RN, Steps, Time\n");

    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        int initial_RN = (rand() % (MAX - MIN + 1)) + MIN;
        int steps;

        clock_t start_time = clock();

        total_lookups++;
        if (cache_get(initial_RN, &steps)) {
            cache_hits++;
        } else {
            steps = collatz_steps(initial_RN);
            cache_put(initial_RN, steps, cache_policy);
        }

        clock_t end_time = clock();
        double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

        printf("RN: %d, Steps: %d, Time: %.5f seconds\n", initial_RN, steps, time_taken);
        fprintf(file, "%d,%d,%f\n", initial_RN, steps, time_taken);
    }

    double hit_percentage = calculate_hit_percentage();
    printf("Overall Cache Hit Percentage: %.2f%%\n", hit_percentage);
    fprintf(file, "Overall Cache Hit Percentage: %.2f%%\n", hit_percentage);

    free(cache);
    fclose(file);
    printf("Results printed to output.csv\n");

    return 0;
}

int collatz_steps(int number) {
    int steps = 0;
    while (number != 1) {
        if (number % 2 == 1) {
            number = (number * 3) + 1;
        } else {
            number = number / 2;
        }
        steps++;
    }
    return steps;
}

int cache_get(int number, int *steps) {
    for (int i = 0; i < current_cache_count; i++) {
        if (cache[i].number == number) {
            *steps = cache[i].steps;
            cache[i].last_used = clock();
            return 1;
        }
    }
    return 0;
}

void cache_put(int number, int steps, const char *policy) {
    if (current_cache_count < cache_size) {
        cache[current_cache_count].number = number;
        cache[current_cache_count].steps = steps;
        cache[current_cache_count].last_used = clock();
        current_cache_count++;
    } else {
        int evict_index = 0;

        if (strcmp(policy, "LRU") == 0) {
            clock_t oldest_time = cache[0].last_used;
            for (int i = 1; i < cache_size; i++) {
                if (cache[i].last_used < oldest_time) {
                    oldest_time = cache[i].last_used;
                    evict_index = i;
                }
            }
        } else if (strcmp(policy, "RR") == 0) {
            evict_index = rand() % cache_size;
        }

        cache[evict_index].number = number;
        cache[evict_index].steps = steps;
        cache[evict_index].last_used = clock();
    }
}

double calculate_hit_percentage() {
    if (total_lookups == 0) {
        return 0.0;
    }
    return ((double)cache_hits / total_lookups) * 100.0;
}
