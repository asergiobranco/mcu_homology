//
//  zero_dimensional_homology.c
//  H0_MCU
//
//  Created by Sérgio Branco on 08/10/2021.
//


#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "zero_dimensional_homology.h"

uint32_t samples_seen[N_SAMPLES/32];
uint32_t connection_matrix[N_SAMPLES][N_SAMPLES/32];

uint32_t ui32_no_simplices;
uint32_t ui32_no_simplices_final;

simplice_t * simplices;

//double distance_matrix[DISTANCE_MATRIX_SIZE];
//uint16_t distance_matrix_order[DISTANCE_MATRIX_SIZE];

 //defines the maximum distance every point is connected

double distance_matrix[DISTANCE_MATRIX_SIZE];

barcode_t * bar_code = NULL;

double distance_between(double * a, double * b){
    double d = 0;
    double p = 0;
    for(int  i = 0; i < N_FEATURES; i++){
        p = pow(a[i] - b[i], 2);
        if(p!=p  || isnan(p)){return 0;}
        d += p;
    }
    d = sqrt(d);
    if(d < 0 || d != d || isnan(d)){
        return 0;
    }else{
        return d;
    }
    
    
}
void build_connection_matrix(double d){
    //uint32_t connection_matrix[N_SAMPLES][N_SAMPLES/32]
    
    uint32_t matrix_idx = 0;
    
    for(int i = 0; i < N_SAMPLES; i++){
        for(int j = 0; j < N_SAMPLES/32; j++){
            connection_matrix[i][j] = (uint32_t)  0;
        }
    }
    
    for(int i = 0; i < N_SAMPLES; i++){
        for(int j = i; j < N_SAMPLES; j++){
            if(i==j){
                connection_matrix[i][j/32] |= ((uint32_t) 1 << (j%32));
            }else{
                if(distance_matrix[matrix_idx] <= d){
                    connection_matrix[i][j/32] |= ((uint32_t) 1 << (j%32));
                    connection_matrix[j][i/32] |= ((uint32_t) 1 << ( i%32));
                    
                }
                matrix_idx++;
            }
        }
    }
    
}

void calculate_distance_matrix(double samples[N_SAMPLES][N_FEATURES]){
    uint32_t matrix_idx = 0;
    for(int i = 0; i < N_SAMPLES; i++){
        for(int j = i+1; j < N_SAMPLES; j++){
            distance_matrix[matrix_idx] = distance_between(samples[i], samples[j]);
            
            matrix_idx++;
        }
    }
}

void clear_simplices(void){
    
    simplice_t * current_simple = simplices;
    simplice_t * next_simple = simplices;
    
    for(int i = 0; i< ui32_no_simplices; i++){
        //printf("%d %p %p\n", count_non_zero(current_simple->simplice), current_simple, current_simple->simplice);
        current_simple =  next_simple;
        next_simple = current_simple->next;
        free(current_simple);
    }
    simplices = NULL;
    ui32_no_simplices = 0;
    
}

void build_bar_code(void){
    
    uint32_t matrix_idx = 0;
    volatile uint8_t save = 1;
    
    double d_MAX_DISTANCE = INFINITY;
    
    double current_distance = 0; //to avoid some errors
    
    calculate_distance_matrix(samples);
    
    matrix_idx = 0;
    
    barcode_t * current_code = NULL;
    
    for(int i = 0; i < N_SAMPLES; i++){
        for(int j = i+1; j < N_SAMPLES; j++){
            
            
            
            current_distance =  distance_matrix[matrix_idx];
            
            build_connection_matrix(distance_matrix[matrix_idx]);
            //printf("dist[%llu] = %f\n", matrix_idx, distance_matrix[matrix_idx+1]);
            distance_matrix[matrix_idx] = current_distance;
            
            
                    
                
            build_simplices();
            merge_simplices();
                //
                // O d_MAX_DISTANCE diz-nos qual o threshold maximo que deu 1 component
                // acima disso nao e necessario fazer o barcode.
                //
               
                if(distance_matrix[matrix_idx] < d_MAX_DISTANCE)
                {
                    if(bar_code == NULL)
                    {
                        bar_code = (barcode_t * )(calloc(1, sizeof(barcode_t)));
                        bar_code->next =  NULL;
                        bar_code->ui_no_complex = ui32_no_simplices;
                        bar_code->d_end_distance = distance_matrix[matrix_idx];
                    }
                    else
                    {
                        
                        save = 1;
                        
                        current_code = bar_code;
                        
                        while ((current_code->next != NULL) && save){
                            
                            if((current_code->ui_no_complex) == ui32_no_simplices_final){
                                //
                                // Se o numero de complex ja existe entao nao o temos de guardar
                                // apenas
                                //
                                save = 0;
                                
                               
                                if((ui32_no_simplices_final == 1) &&  ((current_code->d_end_distance) > distance_matrix[matrix_idx])){
                                    current_code->d_end_distance = distance_matrix[matrix_idx];
                                }
                                else if((current_code->d_end_distance) < distance_matrix[matrix_idx]){
                                    
                                    current_code->d_end_distance = distance_matrix[matrix_idx];
                                }
                                
                            }
                                
                            current_code = current_code->next;
                            
                        }
                        
                        if(save == 1){
                            
                            current_code->next = (barcode_t * )(calloc(1, sizeof(barcode_t)));
                            current_code->next->d_end_distance  = distance_matrix[matrix_idx];
                            current_code->next->ui_no_complex = ui32_no_simplices_final;
                            current_code->next->next = NULL;
                            
                        }
                    }
                    if(ui32_no_simplices_final == 1 && distance_matrix[matrix_idx] > 0){
                        d_MAX_DISTANCE = distance_matrix[matrix_idx];
                        //printf("max dis: %f\n", d_MAX_DISTANCE);
                    }
                }
                
                
                //printf("%f : %lld\n", distance_matrix[matrix_idx], ui32_no_simplices);
                clear_simplices();
            
            
            
            matrix_idx++;
            
        }
    }
    clear_bar_code();
    
}

void clear_bar_code(void){
    
    barcode_t * current_code = bar_code;
    
    while ((current_code->next != NULL)){
        barcode_t * ant = current_code;
        barcode_t * c = current_code->next;
        while ((c->next != NULL)){
            if(c->ui_no_complex == current_code->ui_no_complex){
                if(c->ui_no_complex == 1)
                {
                    if(c->d_end_distance < current_code->d_end_distance){
                        current_code->d_end_distance = c->d_end_distance;
                    }
                }
                else
                {
                    if(c->d_end_distance > current_code->d_end_distance){
                        current_code->d_end_distance = c->d_end_distance;
                    }
                }
                
                ant->next = c->next;
                free(c);
                c = ant;
            }
            ant = c;
            c = c->next;
            
        }
        current_code = current_code->next;
    }
    
}

/*
void clear_bar_code(void){
    
    barcode_t * current_code = bar_code;
    
    while ((current_code->next != NULL)){
        barcode_t * ant = current_code;
        barcode_t * c = current_code->next;
        while ((c->next != NULL)){
            if(c->ui_no_complex == current_code->ui_no_complex){
                if(c->ui_no_complex == 1)
                {
                    if(c->d_end_distance < current_code->d_end_distance){
                        current_code->d_end_distance = c->d_end_distance;
                    }
                }
                else
                {
                    if(c->d_end_distance > current_code->d_end_distance){
                        current_code->d_end_distance = c->d_end_distance;
                    }
                }
                
                ant->next = c->next;
                free(c);
                c = ant;
            }
            c = c->next;
            
        }
        current_code = current_code->next;
    }
    
}
*/
void copy_connection_to_mask(uint32_t * mask, uint32_t *connection_row){
    for(int j = 0; j < (N_SAMPLES/32); j++){
        mask[j] = connection_row[j];
    }
}

void my_logical_or(uint32_t * dest, uint32_t * source){
    for(int j = 0; j < (N_SAMPLES/32); j++){
        dest[j] |= source[j];
    }
}

uint8_t any(uint32_t * a, uint32_t * b){
    for(int j = 0; j < (N_SAMPLES/32); j++){
        
        if(a[j] & b[j]){
            
            return 1;
        }
    }
    return 0;
}

void print(uint32_t * a){
    for(int j = 0; j < (N_SAMPLES); j++){

        //printf("%llu,", 1& (a[j/32]  >> (31  - j%32)));
    }
}

uint16_t count_non_zero(uint32_t  * to_count){
    uint16_t count = 0;
    for(int j = 0; j < N_SAMPLES; j++){
        uint16_t bit = (31-j%32);
        uint16_t byte = j/32;
        if(to_count[byte] & ((uint32_t)1 << bit)){
            count +=1;
        }
    }
    //printf("count: %d", count);
    return count;
}

void append_simplice(uint32_t * mask){
    simplice_t * current_simplice = simplices;
    
 
    simplice_t * last_simplice = NULL;
    
    
    if(ui32_no_simplices == 0)
    {
        simplices = (simplice_t * )calloc(1, sizeof(simplice_t));
        current_simplice = simplices;
    }
    else
    {
        for (int i = 0; i < ui32_no_simplices; i++) {
            last_simplice = current_simplice;
            current_simplice = current_simplice->next;
        }
        current_simplice = (simplice_t * )calloc(1, sizeof(simplice_t));
    }
    
    
    
    if(last_simplice != NULL){
        last_simplice->next = current_simplice;
    }
    current_simplice->next = NULL;
    
    for(int j = 0; j < N_SAMPLES/32; j++){
        current_simplice->simplice[j] = mask[j];
    }
    
    ui32_no_simplices+=1;

}

void build_simplices(void){
    
    
    
    uint32_t mask[N_SAMPLES/32];
    unsigned char get_out = 0;
    simplice_t * current_simplice;
    
    //
    // Clear the samples_seen array
    //
    
    for(int i = 0; i < N_SAMPLES/32; i++){
        samples_seen[i] = (uint32_t) 0;
    }
    
    for(int idx = 0; idx < N_SAMPLES; idx++){
        get_out = 0;
        unsigned int bit =  idx%32;
        unsigned int byte = idx/32;
        
        if(!((samples_seen[byte] & ((uint32_t) 1 << bit)))){
            
            copy_connection_to_mask(mask, connection_matrix[idx]);
            
            //
            // Verifica se algum bit da mask esta no seen, quer dizer que ja foi introduzido
            //
            //*
            if(any(mask, samples_seen)){
                
                current_simplice = simplices;
                for(int i = 0; i < ui32_no_simplices; i++){
                    if(any(mask, current_simplice->simplice)){
                        
                         my_logical_or(current_simplice->simplice, mask);
                         my_logical_or(samples_seen, mask);
                         get_out = 1;
                        
                        current_simplice = current_simplice->next;
                        
                    }
                    
                }
            }
             //*/
            
            if(get_out == 0){
                
                //printf("idx: %d -> ", idx);
                for(int i = idx; i < N_SAMPLES; i++){
                    if(any(mask, connection_matrix[i])){
                       
                        my_logical_or(mask, connection_matrix[i]);
                    }
                }
                my_logical_or(samples_seen, mask);
                
             
                
                if(count_non_zero(mask) > 1){
#ifdef  DEBUG_SIMPLICES_CONNECTIONS
                    printf("::: %d %d \n", idx, count_non_zero(mask));
#endif
                    append_simplice(mask);
                   

                }
            }
            
        }
    }
    
}

/*
 Merges the simplices a last time.
*/
void merge_simplices(void){
    //
    // Stores the points already seen to save processing
    //
    uint32_t * j_seen = (uint32_t *)calloc((ui32_no_simplices/32 +1), sizeof(uint32_t));
    
    //
    // Stores which simplices to connect because they have a common point.
    // It creates a matrix of masks.
    //
    uint32_t ** to_connect = (uint32_t **) calloc((ui32_no_simplices), sizeof(uint32_t * ) );
    for(int i = 0; i < ui32_no_simplices; i++){to_connect[i] = (uint32_t*)calloc((ui32_no_simplices/32 +1), sizeof(uint32_t));}
    
    simplice_t * i_simplice = simplices;
    
    for(int i = 0; i < ui32_no_simplices; i++){
        
        if(!(j_seen[i/32] & ((uint32_t) 1 << (i%32)))){
            simplice_t * j_simplice = simplices;
            for(int j = 0; j < ui32_no_simplices; j++){
                
                if(any(i_simplice->simplice, j_simplice->simplice)){
                    if(j_seen[j/32] & ((uint32_t) 1 << (j%32))){
                        
                        to_connect[j][i/32] |= ((uint32_t) 1 << (i%32));
                    }
                    else{
                        //printf("%d %d\n", i, j);
                        j_seen[j/32] |= ((uint32_t) 1 << (j%32));
                        to_connect[i][j/32] |= ((uint32_t) 1 << (j%32));
                    }
                }
                j_simplice = j_simplice->next;
                
            }
        }
        i_simplice = i_simplice->next;
    }
    
    i_simplice = simplices;
    
    int final_connected  = 0;
    int connected_points = 0;
    
    for(int i = 0; i < ui32_no_simplices; i++){
        
        uint32_t mask[N_SAMPLES/32];
        
        //
        // Clears the mask at each iteration.
        //
        for(int j = 0; j < N_SAMPLES/32; j++){
            mask[j] = (uint32_t) 0;
        }
        
        //
        //
        //
        for(int j = 0; j < ui32_no_simplices; j++){
            //
            // Se for para conectar, connecta-se
            //
            if(to_connect[i][j/32] & ((uint32_t)1<<(j%32))){
                my_logical_or(mask, i_simplice->simplice);
                //connected_points += count_non_zero(mask);
                final_connected+=1;
            }
            
        }
        
        //
        //
        //
        copy_connection_to_mask(i_simplice->simplice, mask);
        int how_many = count_non_zero(i_simplice->simplice);
        if(how_many > 1)
            connected_points += how_many;
        
        //print(mask);
        //printf("\n");
        
        i_simplice = i_simplice->next;
        
        
    }
    
    free(to_connect);
    free(j_seen);
    
    
    
    ui32_no_simplices_final = final_connected + (N_SAMPLES - connected_points);
#ifdef  DEBUG_SIMPLICES_CONNECTIONS
    printf("connnected: %llu - %llu - %d\n", ui32_no_simplices_final, ui32_no_simplices, connected_points);
    i_simplice = simplices;
    for(int j = 0; j < final_connected; j++){
        if(count_non_zero(i_simplice->simplice)){
            print(i_simplice->simplice);
            printf("\n");
        }
        
        i_simplice = i_simplice->next;
    }
#endif
    
}
