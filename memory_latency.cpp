// OS 2025 EX1

#include "memory_latency.h"
#include "measure.h"
#include <ctype.h>
#include <cmath>

#define GALOIS_POLYNOMIAL ((1ULL << 63) | (1ULL << 62) | (1ULL << 60) | (1ULL << 59))
#define NANOESOND_IN_SECOND 1000000000

/**
 * Converts the struct timespec to time in nano-seconds.
 * @param t - the struct timespec to convert.
 * @return - the value of time in nano-seconds.
 */
uint64_t nanosectime(struct timespec t)
{
	// Your code here
    //function works correctly for times up to ~584 years
    return (uint64_t)t.tv_sec * 1000000000ULL + (uint64_t)t.tv_nsec;
    uint64_t ret = t.tv_sec * NANOESOND_IN_SECOND;
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
    repeat = arr_size > repeat ? arr_size:repeat; // Make sure repeat >= arr_size

    // Baseline measurement:
    struct timespec t0;
    timespec_get(&t0, TIME_UTC);
    register uint64_t rnd=0;
    for (register uint64_t i = 0; i < repeat; i++)
    {
        register uint64_t index = i % arr_size;
//        printf("i: %ld, index: %ld, arr_size: %ld, rnd: %ld\n", i, index, arr_size,rnd);
//        if(index >= arr_size||index<0){ printf("ahhhhhhhhhh");}
        rnd ^= index & zero;
    }
    struct timespec t1;
    timespec_get(&t1, TIME_UTC);

    // Memory access measurement:
    struct timespec t2;
    timespec_get(&t2, TIME_UTC);
    rnd=(rnd & zero) ^ 12345;
    for (register uint64_t i = 0; i < repeat; i++)
    {
        register uint64_t index = i % arr_size;
//        printf("i: %ld, index: %ld, arr_size: %ld, rnd: %ld\n", i, index, arr_size,rnd);
//        if(index >= arr_size||index<0){ system("wait"); printf("ahhhhhhhhhh");}
        rnd ^= arr[index] & zero;
    }
    struct timespec t3;
    timespec_get(&t3, TIME_UTC);

    // Calculate baseline and memory access times:
    double baseline_per_cycle=(double)(nanosectime(t1)- nanosectime(t0))/(repeat);
    double memory_per_cycle=(double)(nanosectime(t3)- nanosectime(t2))/(repeat);
    struct measurement result;

    result.baseline = baseline_per_cycle;
    result.access_time = memory_per_cycle;
    result.rnd = rnd;
//    printf("doneeee\n");
    return result;
}

bool validate_args(int argc, char *argv[]); //helper function

void initArray(array_element_t* arr, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        arr[i] = random();
    }
}


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
    float max_size = atof(argv[1]);
    float factor   = atof(argv[2]);
    int repeat   = atoi(argv[3]);

    for(uint64_t i=100; i<max_size; i*=factor)
    {
        array_element_t* arr = (array_element_t*)malloc(i* sizeof(array_element_t));
        if(!arr)
        {
            fprintf(stderr, "Allocation error\n");
            return 1;
        }
        initArray(arr, i);
//        printf("beforeeeeee\n");
        measurement result = measure_latency(repeat, arr, i, zero);
//        printf("inbetweeennnnnnn\n");
        measurement seq_result = measure_sequential_latency(repeat, arr, ceil(i), zero);
        int offset = result.access_time - result.baseline;
        int offset_sequential = seq_result.access_time - seq_result.baseline;
        printf("%lu,%d,%d\n", uint64_t(ceil(i)), offset, offset_sequential);
        free(arr);
//        printf("freeeeeeeed\n");
    }
    return 0;
}


int is_positive_float(const char *str) {
    if (str == NULL || *str == '\0') return 0; // Null or empty string is invalid

    int has_dot = 0;
    int has_digit = 0;

    if (*str == '.') return 0; // Cannot start with a dot

    for (; *str; str++) {
        if (*str == '.') {
            if (has_dot) return 0; // Multiple dots are not allowed
            has_dot = 1;
        } else if (isdigit((unsigned char)*str)) {
            has_digit = 1;
        } else {
            return 0; // Invalid character
        }
    }

    return has_digit; // Must have at least one digit
}

bool validate_args(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "The number of arguments is not 3 but: %d\n", argc - 1);
        return false;
    }

    if (!is_positive_float(argv[1]) || !is_positive_float(argv[2]) || !is_positive_float(
            argv[3])) {
        fprintf(stderr, "All arguments must be positive integers.\n");
        return false;
    }

    float max_size = atof(argv[1]);
    float factor   = atof(argv[2]);
    float repeat   = atof(argv[3]);

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

