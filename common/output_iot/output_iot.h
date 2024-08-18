#ifndef OUTPUT_IO_H
#define OUTPUT_IO_H
#include "esp_err.h"
#include "hal/gpio_types.h"
  
typedef void (*input_callback_t) (int);
typedef enum{
    LO_TO_HI = 1,
    HI_TO_LO = 2,
    ANY_EDLE = 3
} interrupt_type_edle_t;

 void output_io_create(gpio_num_t gpio_num);
 void output_io_set_level (gpio_num_t gpio_num, int level);
 void output_io_toggle (gpio_num_t gpio_num);

#endif