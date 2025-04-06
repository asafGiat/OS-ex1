// OS 2025 EX1

#include <stdbool.h>
#include <ctype.h>

#include "memory_latency.h"
#include "measure.h"

#define GALOIS_POLYNOMIAL ((1ULL << 63) | (1ULL << 62) | (1ULL << 60) | (1ULL << 59))

#define NANOESOND_IN_SCEOND 1000000

/**
 * Converts the struct timespec to time in nano-seconds.
 * @param t - the struct timespec to convert.
 * @return - the value of time in nano-seconds.
 */
uint64_t nanosectime(struct timespec t)
{
	//function works correctly for times up to ~584 years
    uint64_t ret = t.tv_sec * NANOESOND_IN_SCEOND;
    ret += t.tv_nsec;
    return ret;
}

/**
* Measures the average latency of accessing a given array in a sequential order.
* @param repeat - the number of times to repeat the measurement for and average on.
* @param arr - an allocated (not empty) array to preform measurement on.
* @param arr_size - the length of the array arr.
* @param zero - a variable containing zero in a way that the compiler doesn't "know" it in compilation time.
* @return struct measurement containing the measurement with the following fields:
*      double baseline - the average time (ns) taken to preform the measured operation without memory access.
*      double access_time - the average time (ns) taken to preform the measured operation with memory access.
*      uint64_t rnd - the variable used to randomly access the array, returned to prevent compiler optimizations.
*/
struct measurement measure_sequential_latency(uint64_t repeat, array_element_t* arr, uint64_t arr_size, uint64_t zero)
{
    // Your code here
}


bool validate_args(int argc, char *argv[]); //helper function

/**
 * Runs the logic of the memory_latency program. Measures the access latency for random and sequential memory access
 * patterns.
 * Usage: './memory_latency max_size factor repeat' where:
 *      - max_size - the maximum size in bytes of the array to measure access latency for.
 *      - factor - the factor in the geometric series representing the array sizes to check.
 *      - repeat - the number of times each measurement should be repeated for and averaged on.
 * The program will print output to stdout in the following format:
 *      mem_size_1,offset_1,offset_sequential_1
 *      mem_size_2,offset_2,offset_sequential_2
 *              ...
 *              ...
 *              ...
 */
int main(int argc, char* argv[])
{
    // zero==0, but the compiler doesn't know it. Use as the zero arg of measure_latency and measure_sequential_latency.
    struct timespec t_dummy;
    timespec_get(&t_dummy, TIME_UTC);
    const uint64_t zero = nanosectime(t_dummy)>1000000000ull?0:nanosectime(t_dummy);

    // Your code here
    if (!validate_args(argc, argv)) {
        return EXIT_FAILURE;
    }
    int max_size = atoi(argv[1]);
    int factor   = atoi(argv[2]);
    int repeat   = atoi(argv[3]);

    for(int i=1; i<max_size; i*=factor)
    {
        array_element_t* arr = (array_element_t*)malloc(i);
        if(!arr)
        {
            fprintf(stderr, "Allocation error\n");
            return 1;
        }
        measurement result = measure_latency(repeat, arr, i, zero);
        measurement seq_result = measure_sequential_latency(repeat, arr, i, zero);
        int offset = result.access_time - result.baseline;
        int offset_sequential = seq_result.access_time - seq_result.baseline;
        printf("%d,%d,%d", i, result, offset, offset_sequential);
        free(arr);
    }
    return 0;
}

bool is_positive_integer(const char *str) {
    if (str == NULL || *str == '\0') return false;
    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

bool validate_args(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s max_size factor repeat\n", argv[0]);
        return false;
    }

    if (!is_positive_integer(argv[1]) || !is_positive_integer(argv[2]) || !is_positive_integer(argv[3])) {
        fprintf(stderr, "All arguments must be positive integers.\n");
        return false;
    }

    int max_size = atoi(argv[1]);
    int factor   = atoi(argv[2]);
    int repeat   = atoi(argv[3]);

    if (max_size <= 0 || factor <= 1 || repeat <= 0) {
        fprintf(stderr,
            "Invalid values:\n"
            "- max_size must be > 0\n"
            "- factor must be > 1\n"
            "- repeat must be > 0\n");
        return false;
    }

    return true;
}

