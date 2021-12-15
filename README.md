# mcu_homology
Persisten Homology Analysis for Resource-Scarce Embedded Systems


# How to run

```C
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "mcu_homology/dataset_info.h"
#include "mcu_homology/zero_dimensional_homology.h"
#include "dataset.h"

/*
  Before running, go to mcu_homology/dataset_info.h and change the number of samples and features. 
*/


int main(int argc, const char * argv[]) {

    //
    // Builds the bar code and stores it into bar_code
    //
    build_bar_code();
    
    //
    // bar_code is a linked list
    //
    barcode_t * current_code = bar_code;
    
    
    while ((current_code->next != NULL)){
         printf("[%d, %f],\n", current_code->ui_no_complex, current_code->d_end_distance);
         current_code = current_code->next;
    }
   
}
```
