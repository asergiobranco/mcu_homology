//
//  dataset_info.h
//  H0_MCU
//
//  Created by Sérgio Branco on 30/09/2021.
//

#ifndef dataset_info_h
#define dataset_info_h

#include <math.h>

#define N_SAMPLES 128
#define N_FEATURES 4

//#define DEBUG_SIMPLICES_CONNECTIONS 

#if N_SAMPLES == 512
    #define DISTANCE_MATRIX_SIZE 130816
#elif N_SAMPLES == 256
    #define DISTANCE_MATRIX_SIZE 32640
#elif N_SAMPLES == 128
    #define DISTANCE_MATRIX_SIZE 8128
#elif N_SAMPLES == 64
    #define DISTANCE_MATRIX_SIZE 2016
#elif N_SAMPLES == 32
    #define DISTANCE_MATRIX_SIZE 496
#endif

extern double samples[N_SAMPLES][N_FEATURES];

#endif /* dataset_info_h */
