#include "cucumber.h"

void datatable_free(DataTable *dt) {
    if (dt == NULL) return;
    
    int32_t total = dt->rows * dt->cols;
    
    for (int32_t i = 0; i < total; i++) {
        free(dt->cells[i]);
        dt->cells[i] = NULL;  
    }
    
    free(dt->cells); 
    dt->cells = NULL;
    
    free(dt); 
    dt = NULL;                
}