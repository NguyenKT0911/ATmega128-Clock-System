#define F_CPU 8000000UL

#include <avr/io.h>
#include <stdint.h>

#include "lcd.h"
#include "button.h"
#include "buzzer.h"
#include "timer.h"
#include "i2c.h"
#include "ds1307.h"
#include "onewire.h"
#include "ds18b20.h"


// ============================================================
// MODE
// ============================================================

#define MODE_NORMAL      0
#define MODE_SET_TIME    1
#define MODE_SET_ALARM   2
#define MODE_SET_TEMP    3


// ============================================================
// GLOBAL VARIABLES
// ============================================================

DS1307_Time rtc_time;

int16_t temperature = 0;


// Alarm mac dinh: 07:30
uint8_t alarm_hour   = 7;
uint8_t alarm_minute = 30;


// Nguong nhiet do mac dinh: 35.0 C
// Don vi: 0.1 do C
int16_t temp_limit = 350;


// Mode hien tai
uint8_t mode = MODE_NORMAL;


// Field dang chinh
//
// SET TIME:
// 0 = hour
// 1 = minute
// 2 = second
// 3 = date
// 4 = month
// 5 = year
//
// SET ALARM:
// 0 = hour
// 1 = minute
uint8_t field = 0;


// ============================================================
// LCD HELPER
// ============================================================

void lcd_put2(uint8_t number)
{
    lcd_putc((number / 10) + '0');
    lcd_putc((number % 10) + '0');
}


// temperature co don vi 0.1 C
//
// 315  -> 31.5
// -55  -> -5.5
void lcd_put_temperature(int16_t value)
{
    uint16_t temp;
    uint8_t integer_part;
    uint8_t decimal_part;


    if (value < 0)
    {
        lcd_putc('-');
        temp = (uint16_t)(-value);
    }
    else
    {
        temp = (uint16_t)value;
    }


    integer_part = temp / 10;
    decimal_part = temp % 10;


    if (integer_part >= 100)
    {
        lcd_putc((integer_part / 100) + '0');
    }

    if (integer_part >= 10)
    {
        lcd_putc(((integer_part / 10) % 10) + '0');
    }

    lcd_putc((integer_part % 10) + '0');

    lcd_putc('.');

    lcd_putc(decimal_part + '0');
}


// ============================================================
// NORMAL DISPLAY
// ============================================================

void display_normal(void)
{
    // --------------------------------------------------------
    // Row 0: TIME
    // --------------------------------------------------------

    lcd_gotoxy(0, 0);

    lcd_puts("TIME: ");

    lcd_put2(rtc_time.hour);
    lcd_putc(':');

    lcd_put2(rtc_time.minute);
    lcd_putc(':');

    lcd_put2(rtc_time.second);

    lcd_puts("      ");


    // --------------------------------------------------------
    // Row 1: DATE
    // --------------------------------------------------------

    lcd_gotoxy(0, 1);

    lcd_puts("DATE: ");

    lcd_put2(rtc_time.date);
    lcd_putc('/');

    lcd_put2(rtc_time.month);
    lcd_putc('/');

    lcd_puts("20");
    lcd_put2(rtc_time.year);

    lcd_puts("  ");


    // --------------------------------------------------------
    // Row 2: TEMPERATURE
    // --------------------------------------------------------

    lcd_gotoxy(0, 2);

    lcd_puts("TEMP: ");

    lcd_put_temperature(temperature);

    lcd_puts(" C");

    lcd_puts("        ");


    // --------------------------------------------------------
    // Row 3: ALARM + LIMIT
    // --------------------------------------------------------

    lcd_gotoxy(0, 3);

    lcd_puts("A:");

    lcd_put2(alarm_hour);

    lcd_putc(':');

    lcd_put2(alarm_minute);

    lcd_puts(" L:");

    lcd_put_temperature(temp_limit);

    lcd_putc('C');

    lcd_puts(" ");
}


// ============================================================
// SET TIME DISPLAY
// ============================================================

void display_set_time(void)
{
    lcd_gotoxy(0, 0);
    lcd_puts("SET TIME            ");


    // Time
    lcd_gotoxy(0, 1);

    lcd_put2(rtc_time.hour);
    lcd_putc(':');

    lcd_put2(rtc_time.minute);
    lcd_putc(':');

    lcd_put2(rtc_time.second);

    lcd_puts("            ");


    // Date
    lcd_gotoxy(0, 2);

    lcd_put2(rtc_time.date);
    lcd_putc('/');

    lcd_put2(rtc_time.month);
    lcd_putc('/');

    lcd_puts("20");

    lcd_put2(rtc_time.year);

    lcd_puts("          ");


    // Field
    lcd_gotoxy(0, 3);

    switch (field)
    {
        case 0:
            lcd_puts("FIELD: HOUR         ");
            break;

        case 1:
            lcd_puts("FIELD: MINUTE       ");
            break;

        case 2:
            lcd_puts("FIELD: SECOND       ");
            break;

        case 3:
            lcd_puts("FIELD: DATE         ");
            break;

        case 4:
            lcd_puts("FIELD: MONTH        ");
            break;

        case 5:
            lcd_puts("FIELD: YEAR         ");
            break;
    }
}


// ============================================================
// SET ALARM DISPLAY
// ============================================================

void display_set_alarm(void)
{
    lcd_gotoxy(0, 0);
    lcd_puts("SET ALARM           ");


    lcd_gotoxy(0, 1);

    lcd_puts("ALARM: ");

    lcd_put2(alarm_hour);

    lcd_putc(':');

    lcd_put2(alarm_minute);

    lcd_puts("         ");


    lcd_gotoxy(0, 2);

    if (field == 0)
    {
        lcd_puts("FIELD: HOUR         ");
    }
    else
    {
        lcd_puts("FIELD: MINUTE       ");
    }


    lcd_gotoxy(0, 3);
    lcd_puts("SET:FIELD MODE:NEXT ");
}


// ============================================================
// SET TEMPERATURE LIMIT DISPLAY
// ============================================================

void display_set_temp(void)
{
    lcd_gotoxy(0, 0);
    lcd_puts("SET TEMP LIMIT      ");


    lcd_gotoxy(0, 1);

    lcd_puts("LIMIT: ");

    lcd_put_temperature(temp_limit);

    lcd_puts(" C");

    lcd_puts("        ");


    lcd_gotoxy(0, 2);
    lcd_puts("UP/DOWN: CHANGE     ");


    lcd_gotoxy(0, 3);
    lcd_puts("MODE: NORMAL        ");
}


// ============================================================
// INCREASE TIME FIELD
// ============================================================

void increase_time_field(void)
{
    switch (field)
    {
        case 0:

            rtc_time.hour++;

            if (rtc_time.hour > 23)
                rtc_time.hour = 0;

            break;


        case 1:

            rtc_time.minute++;

            if (rtc_time.minute > 59)
                rtc_time.minute = 0;

            break;


        case 2:

            rtc_time.second++;

            if (rtc_time.second > 59)
                rtc_time.second = 0;

            break;


        case 3:

            rtc_time.date++;

            if (rtc_time.date > 31)
                rtc_time.date = 1;

            break;


        case 4:

            rtc_time.month++;

            if (rtc_time.month > 12)
                rtc_time.month = 1;

            break;


        case 5:

            rtc_time.year++;

            if (rtc_time.year > 99)
                rtc_time.year = 0;

            break;
    }
}


// ============================================================
// DECREASE TIME FIELD
// ============================================================

void decrease_time_field(void)
{
    switch (field)
    {
        case 0:

            if (rtc_time.hour == 0)
                rtc_time.hour = 23;
            else
                rtc_time.hour--;

            break;


        case 1:

            if (rtc_time.minute == 0)
                rtc_time.minute = 59;
            else
                rtc_time.minute--;

            break;


        case 2:

            if (rtc_time.second == 0)
                rtc_time.second = 59;
            else
                rtc_time.second--;

            break;


        case 3:

            if (rtc_time.date <= 1)
                rtc_time.date = 31;
            else
                rtc_time.date--;

            break;


        case 4:

            if (rtc_time.month <= 1)
                rtc_time.month = 12;
            else
                rtc_time.month--;

            break;


        case 5:

            if (rtc_time.year == 0)
                rtc_time.year = 99;
            else
                rtc_time.year--;

            break;
    }
}


// ============================================================
// PROCESS BUTTONS
// ============================================================

void process_buttons(void)
{
    // --------------------------------------------------------
    // MODE
    // --------------------------------------------------------

    if (button_mode_pressed())
    {
        // Neu dang SET TIME va sap thoat khoi SET TIME
        // thi luu thoi gian vao DS1307
        if (mode == MODE_SET_TIME)
        {
            ds1307_set_time(&rtc_time);
        }


        mode++;

        field = 0;


        if (mode > MODE_SET_TEMP)
        {
            mode = MODE_NORMAL;
        }


        lcd_clear();

        return;
    }


    // --------------------------------------------------------
    // NORMAL MODE
    // --------------------------------------------------------

    if (mode == MODE_NORMAL)
    {
        return;
    }


    // --------------------------------------------------------
    // SET
    // --------------------------------------------------------

    if (button_set_pressed())
    {
        if (mode == MODE_SET_TIME)
        {
            field++;

            if (field > 5)
            {
                field = 0;
            }
        }

        else if (mode == MODE_SET_ALARM)
        {
            field++;

            if (field > 1)
            {
                field = 0;
            }
        }
    }


    // --------------------------------------------------------
    // UP
    // --------------------------------------------------------

    if (button_up_pressed() || button_up_repeat())
    {
        // SET TIME
        if (mode == MODE_SET_TIME)
        {
            increase_time_field();
        }


        // SET ALARM
        else if (mode == MODE_SET_ALARM)
        {
            if (field == 0)
            {
                alarm_hour++;

                if (alarm_hour > 23)
                {
                    alarm_hour = 0;
                }
            }
            else
            {
                alarm_minute++;

                if (alarm_minute > 59)
                {
                    alarm_minute = 0;
                }
            }
        }


        // SET TEMP LIMIT
        else if (mode == MODE_SET_TEMP)
        {
            // Tang 1.0 C
            temp_limit += 10;

            if (temp_limit > 1250)
            {
                temp_limit = 1250;
            }
        }
    }


    // --------------------------------------------------------
    // DOWN
    // --------------------------------------------------------

    if (button_down_pressed() || button_down_repeat())
    {
        // SET TIME
        if (mode == MODE_SET_TIME)
        {
            decrease_time_field();
        }


        // SET ALARM
        else if (mode == MODE_SET_ALARM)
        {
            if (field == 0)
            {
                if (alarm_hour == 0)
                {
                    alarm_hour = 23;
                }
                else
                {
                    alarm_hour--;
                }
            }
            else
            {
                if (alarm_minute == 0)
                {
                    alarm_minute = 59;
                }
                else
                {
                    alarm_minute--;
                }
            }
        }


        // SET TEMP LIMIT
        else if (mode == MODE_SET_TEMP)
        {
            // Giam 1.0 C
            temp_limit -= 10;

            if (temp_limit < -550)
            {
                temp_limit = -550;
            }
        }
    }
}


// ============================================================
// RTC TASK
// ============================================================

void rtc_task(uint32_t now)
{
    static uint32_t last_read = 0;


    // Doc DS1307 moi 200 ms
    if ((now - last_read) >= 200)
    {
        last_read = now;


        // Khi dang chinh time thi KHONG doc DS1307
        // Neu doc se ghi de gia tri dang chinh
        if (mode != MODE_SET_TIME)
        {
            ds1307_get_time(&rtc_time);
        }
    }
}


// ============================================================
// TEMPERATURE TASK
// ============================================================

void temperature_task(uint32_t now)
{
    static uint8_t state = 0;

    static uint32_t conversion_start = 0;


    // --------------------------------------------------------
    // STATE 0
    // Start conversion
    // --------------------------------------------------------

    if (state == 0)
    {
        if (ds18b20_start_conversion())
        {
            conversion_start = now;

            state = 1;
        }
    }


    // --------------------------------------------------------
    // STATE 1
    // Wait 750 ms
    // KHONG BLOCK CPU
    // --------------------------------------------------------

    else if (state == 1)
    {
        if ((now - conversion_start) >= 750)
        {
            ds18b20_read_temperature(&temperature);

            state = 0;
        }
    }
}


// ============================================================
// CHECK ALARM
// ============================================================

void check_alarm(void)
{
    uint8_t alarm_active = 0;


    // --------------------------------------------------------
    // Time alarm
    // --------------------------------------------------------

    if ((rtc_time.hour == alarm_hour) &&
        (rtc_time.minute == alarm_minute))
    {
        alarm_active = 1;
    }


    // --------------------------------------------------------
    // Temperature alarm
    // --------------------------------------------------------

    if (temperature >= temp_limit)
    {
        alarm_active = 1;
    }


    // --------------------------------------------------------
    // Buzzer
    // --------------------------------------------------------

    if (alarm_active)
    {
        buzzer_on();
    }
    else
    {
        buzzer_off();
    }
}


// ============================================================
// DISPLAY TASK
// ============================================================

void display_task(uint32_t now)
{
    static uint32_t last_display = 0;


    // Update LCD moi 100 ms
    if ((now - last_display) < 100)
    {
        return;
    }


    last_display = now;


    switch (mode)
    {
        case MODE_NORMAL:

            display_normal();

            break;


        case MODE_SET_TIME:

            display_set_time();

            break;


        case MODE_SET_ALARM:

            display_set_alarm();

            break;


        case MODE_SET_TEMP:

            display_set_temp();

            break;
    }
}


// ============================================================
// MAIN
// ============================================================

int main(void)
{
    uint32_t now;


    // ========================================================
    // INITIALIZATION
    // ========================================================

    lcd_init();

    button_init();

    buzzer_init();

    i2c_init();

    timer_init();


    buzzer_off();

    lcd_clear();


    // ========================================================
    // Read RTC lan dau
    // ========================================================

    ds1307_get_time(&rtc_time);


    // ========================================================
    // MAIN LOOP
    // ========================================================

    while (1)
    {
        // ----------------------------------------------------
        // Lay system time
        // ----------------------------------------------------

        now = timer_millis();


        // ----------------------------------------------------
        // BUTTON
        // ----------------------------------------------------

        button_update(now);

        process_buttons();


        // ----------------------------------------------------
        // RTC
        // ----------------------------------------------------

        rtc_task(now);


        // ----------------------------------------------------
        // DS18B20
        // ----------------------------------------------------

        temperature_task(now);


        // ----------------------------------------------------
        // ALARM
        // ----------------------------------------------------

        check_alarm();


        // ----------------------------------------------------
        // LCD
        // ----------------------------------------------------

        display_task(now);
    }


    return 0;
}