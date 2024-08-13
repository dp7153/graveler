#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>  // Include for uint32_t and uint64_t

#ifdef _WIN32
#include <process.h> // for getpid() on Windows
#else
#include <unistd.h>  // for getpid() on Unix/Linux
#endif


/*--------------------------------------------------*/
/*                  compilation:                    */
/*          gcc -O3 graveler.c -o graveler          */
/*--------------------------------------------------*/


// PCG32 PRNG implementation
typedef struct {
    uint64_t state;
    uint64_t inc;
} pcg32_random_t;

// Initialize the PCG with a seed
void pcg32_srandom(pcg32_random_t *rng, uint64_t initstate, uint64_t initseq) {
    rng->state = 0U;
    rng->inc = (initseq << 1u) | 1u;
    rng->state = rng->state * 6364136223846793005ULL + rng->inc;
    rng->state += initstate;
    rng->state = rng->state * 6364136223846793005ULL + rng->inc;
}

// Generate a random number using PCG
uint32_t pcg32_random_r(pcg32_random_t *rng) {
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

#define NUMBER_OF_FACES 4

int main(){
    unsigned int seed = time(NULL) ^ (getpid() << 16);
    
    pcg32_random_t rng;
    pcg32_srandom(&rng, seed, seed + 1);  // Changed to ensure proper initialization

    int numberOfDice = 231;
    int numberOfFaces = 4;
    int throw = 0;
    int numberOfThrows = 0;
    int bestIteration = 0;

    printf("How many throws? ");
    scanf("%d", &numberOfThrows);
    printf("\n");

    clock_t start_time = clock();

    int faces[NUMBER_OF_FACES] = {0};
    int highest[NUMBER_OF_FACES] = {0};

    while(highest[0] < 177 && throw < numberOfThrows){
        for(int i = 0; i < numberOfDice; i++){
            int lands = pcg32_random_r(&rng) % numberOfFaces;
            faces[lands]++;
        }
        if(faces[0] > highest[0]){
            for(int i = 0; i < numberOfFaces; i++){
                highest[i] = faces[i];
            }
            bestIteration = throw + 1;
        }
        for(int i = 0; i < numberOfFaces; i++){
            faces[i] = 0;
        }
        throw++;
    }

    printf("Best iteration: #%d\n", bestIteration);
    for(int i = 0; i < numberOfFaces; i++){
        printf("%d: %d\n", i + 1, highest[i]);
    }

    // End time
    clock_t end_time = clock();

    // Calculate the time taken in seconds
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Time taken: %.6f seconds\n", time_spent);

    return 0;
}
