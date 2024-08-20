#define UH2 20
#define VH2 19
#define WH2 18
#define UL2 17
#define VL2 16
#define WL2 15
#define UH1 14
#define VH1 12
#define WH1 11
#define UL1 10
#define VL1 9
#define WL1 8

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

void init_gpio_pins(void){
    for(int i = 0; i < 20-8+1;i++){
        gpio_init(i + 8);
        gpio_set_dir(i + 8, GPIO_OUT);
    }
}

void set_pins(char U, char V, char W){
    const uint32_t mask = 0b111111111111100000000;
    uint32_t value = 0;
    
    char pin_u = 3 << U;
    char pin_v = 3 << V;
    char pin_w = 3 << W;

    // pin u
    value |= ((pin_u & 0b1000) > 0) << UH2;
    value |= ((pin_u & 0b0001) > 0) << UL2;
    value |= ((pin_u & 0b0100) > 0) << UH1;
    value |= ((pin_u & 0b0010) > 0) << UL1;
    
    // pin v
    value |= ((pin_v & 0b1000) > 0) << VH2;
    value |= ((pin_v & 0b0001) > 0) << VL2;
    value |= ((pin_v & 0b0100) > 0) << VH1;
    value |= ((pin_v & 0b0010) > 0) << VL1;

    // pin w
    value |= ((pin_w & 0b1000) > 0) << WH2;
    value |= ((pin_w & 0b0001) > 0) << WL2;
    value |= ((pin_w & 0b0100) > 0) << WH1;
    value |= ((pin_w & 0b0010) > 0) << WL1;

    gpio_put_masked(mask, value);
}