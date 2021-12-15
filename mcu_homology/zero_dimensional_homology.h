//
//  zero_dimensional_homology.h
//  H0_MCU
//
//  Created by Sérgio Branco on 08/10/2021.
//

#ifndef zero_dimensional_homology_h
#define zero_dimensional_homology_h

#include <stdio.h>
#include "dataset_info.h"

typedef struct simplice_k{
    struct simplice_k * next;
    uint32_t simplice[N_SAMPLES/32];
} simplice_t;

typedef struct br{
    struct br * next;
    double d_end_distance;
    uint32_t ui_no_complex;
}  barcode_t;


extern uint32_t samples_seen[N_SAMPLES/32];
extern uint32_t connection_matrix[N_SAMPLES][N_SAMPLES/32];

extern uint32_t ui32_no_simplices;
extern uint32_t ui32_no_simplices_final;

extern simplice_t * simplices;

//double distance_matrix[DISTANCE_MATRIX_SIZE];
//uint16_t distance_matrix_order[DISTANCE_MATRIX_SIZE];

 //defines the maximum distance every point is connected

extern double distance_matrix[DISTANCE_MATRIX_SIZE];

extern barcode_t * bar_code;

void build_simplices(void);
void copy_connection_to_mask(uint32_t *, uint32_t *);
void my_logical_or(uint32_t * , uint32_t *);
void append_simplice(uint32_t *);
void calculate_distance_matrix(double samples[N_SAMPLES][N_FEATURES]);
void merge_simplices(void);
double distance_between(double * a, double * b);
void build_connection_matrix(double d);

/*
    Clears the bar code to solve the problem of some repetitions. IDKW!
    The functions deletes the nodes which have the same number of complexes.
    Leaves the correct distance.
*/
void clear_bar_code(void);
void build_bar_code(void);

#endif /* zero_dimensional_homology_h */
