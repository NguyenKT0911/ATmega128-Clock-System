#include <avr/io.h>
#include <stdint.h>

#include "button.h"

#define DEBOUNCE_TIME       20
#define HOLD_TIME           500
#define REPEAT_TIME         100


static uint8_t stable_state = 0x0F;
static uint8_t last_raw     = 0x0F;

static uint8_t press_event  = 0;
static uint8_t repeat_event = 0;

static uint32_t debounce_time = 0;

static uint32_t up_press_time = 0;
static uint32_t up_repeat_time = 0;

static uint32_t down_press_time = 0;
static uint32_t down_repeat_time = 0;


// ============================================================
// INIT
// ============================================================

void button_init(void)
{
    BUTTON_DDR &= ~(
        (1 << BTN_MODE) |
        (1 << BTN_SET)  |
        (1 << BTN_UP)   |
        (1 << BTN_DOWN)
    );

    // External pull-up da co tren mach
    BUTTON_PORT &= ~(
        (1 << BTN_MODE) |
        (1 << BTN_SET)  |
        (1 << BTN_UP)   |
        (1 << BTN_DOWN)
    );
}


// ============================================================
// BUTTON UPDATE
// Goi lien tuc trong main loop
// ============================================================

void button_update(uint32_t now)
{
    uint8_t raw;

    raw = BUTTON_PIN & 0x0F;


    // --------------------------------------------------------
    // Debounce
    // --------------------------------------------------------

    if (raw != last_raw)
    {
        last_raw = raw;
        debounce_time = now;
    }


    if ((now - debounce_time) >= DEBOUNCE_TIME)
    {
        if (raw != stable_state)
        {
            uint8_t old_state;

            old_state = stable_state;
            stable_state = raw;


            // HIGH -> LOW = vua nhan
            uint8_t pressed;

            pressed = old_state & (~stable_state);


            press_event |= pressed;


            // UP vua duoc nhan
            if (pressed & (1 << BTN_UP))
            {
                up_press_time = now;
                up_repeat_time = now;
            }


            // DOWN vua duoc nhan
            if (pressed & (1 << BTN_DOWN))
            {
                down_press_time = now;
                down_repeat_time = now;
            }
        }
    }


    // ========================================================
    // HOLD UP
    // ========================================================

    if (!(stable_state & (1 << BTN_UP)))
    {
        // Da giu >= 500 ms?
        if ((now - up_press_time) >= HOLD_TIME)
        {
            // Lap moi 100 ms
            if ((now - up_repeat_time) >= REPEAT_TIME)
            {
                up_repeat_time = now;

                repeat_event |= (1 << BTN_UP);
            }
        }
    }


    // ========================================================
    // HOLD DOWN
    // ========================================================

    if (!(stable_state & (1 << BTN_DOWN)))
    {
        if ((now - down_press_time) >= HOLD_TIME)
        {
            if ((now - down_repeat_time) >= REPEAT_TIME)
            {
                down_repeat_time = now;

                repeat_event |= (1 << BTN_DOWN);
            }
        }
    }
}


// ============================================================
// PRESS EVENT
// ============================================================

static uint8_t get_press_event(uint8_t pin)
{
    uint8_t mask;

    mask = (1 << pin);

    if (press_event & mask)
    {
        press_event &= ~mask;

        return 1;
    }

    return 0;
}


// ============================================================
// REPEAT EVENT
// ============================================================

static uint8_t get_repeat_event(uint8_t pin)
{
    uint8_t mask;

    mask = (1 << pin);

    if (repeat_event & mask)
    {
        repeat_event &= ~mask;

        return 1;
    }

    return 0;
}


// ============================================================
// NORMAL PRESS
// ============================================================

uint8_t button_mode_pressed(void)
{
    return get_press_event(BTN_MODE);
}


uint8_t button_set_pressed(void)
{
    return get_press_event(BTN_SET);
}


uint8_t button_up_pressed(void)
{
    return get_press_event(BTN_UP);
}


uint8_t button_down_pressed(void)
{
    return get_press_event(BTN_DOWN);
}


// ============================================================
// AUTO REPEAT
// ============================================================

uint8_t button_up_repeat(void)
{
    return get_repeat_event(BTN_UP);
}


uint8_t button_down_repeat(void)
{
    return get_repeat_event(BTN_DOWN);
}


// ============================================================
// HELD STATE
// ============================================================

uint8_t button_up_held(void)
{
    return !(stable_state & (1 << BTN_UP));
}


uint8_t button_down_held(void)
{
    return !(stable_state & (1 << BTN_DOWN));
}