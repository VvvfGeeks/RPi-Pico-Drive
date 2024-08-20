/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/structs/systick.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum
{
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

char __flash_binary_end;

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

static void led_blinking_task(void);
static void cdc_task(void);
extern void init_gpio_pins(void);
extern void set_pins(char U, char V, char W);

/*------------- MAIN -------------*/

#define buff_len 40000
static char buff[buff_len] = {0};
static int buff_read = 0;
static int buff_write = buff_len - 64;

void reset_buffer()
{
  buff_read = 0;
  buff_write = buff_len - 64;
  memset(buff, 0, sizeof(buff));
}

void core1_entry(void)
{
  while (true)
  {
    while (tud_cdc_n_available(0) == false)
      ;
    systick_hw->csr = 1;
    systick_hw->rvr = 4;
    systick_hw->cvr = 0;
    while (true)
    {
      while (systick_hw->cvr > 0)
        ;

      if (buff_read == buff_write)
      {
        set_pins(4, 4, 4);
        continue;
      }

      char read = buff[buff_read];
      buff[buff_read++] = 0;
      
      if (buff_read == buff_len)
        buff_read = 0;

      if (read == 0xFF)
      {
        set_pins(4, 4, 4);
        reset_buffer();
        break;
      }

      set_pins((read & 0b110000) >> 4, (read & 0b001100) >> 2, (read & 0b000011));

      while (systick_hw->cvr == 0)
        ;
    }
  }
}

int main(void)
{

  set_sys_clock_khz(250000, true);

  board_init();
  tud_init(0);
  init_gpio_pins();

  multicore_launch_core1(core1_entry);

  while (1)
  {
    tud_task(); // tinyusb device task
    cdc_task();
    led_blinking_task();
  }
}

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
static void cdc_task(void)
{
  if (tud_cdc_n_available(0))
  {
    uint8_t buf[2048];
    uint32_t count = tud_cdc_n_read(0, buf, sizeof(buf));

    for (int i = 0; i < count; i++)
    {
      buff[buff_write++] = buf[i];
      if (buff_write == buff_len)
        buff_write = 0;
    }
  }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if (board_millis() - start_ms < blink_interval_ms)
    return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}