#include "stm8s.h"
#include "stdio.h"
#include "encoder.h"
#include "milis.h"
#include "LCD_I2C.h"

#define RGB_PORT    GPIOD
#define RED_PIN     GPIO_PIN_3
#define GRN_PIN     GPIO_PIN_5
#define BLU_PIN     GPIO_PIN_6
#define OFF         GPIO_WriteLow
#define ON          GPIO_WriteHigh
#define micros_init milis_init
#define micros      milis

void stm8_init(void);
void RGB_PWM(void);
void ENC_clr_change(void);
void ENC_clr_minus(void);
void ENC_clr_plus(void);
void counter_reset(void);
void LCD_update(void);

uint32_t master_time            = 0;
uint8_t  master_period          = 1; 
uint8_t  counter[4]             = {0, 0, 0, 0};         // master, R, G, B
int8_t   counter_previous[4]    = {0, 0, 0, 0};         // master, R, G, B
int8_t   counter_reference[4]   = {0, 0, 0, 0};         // master, R, G, B
uint8_t  cycles                 = 0;
uint8_t  index                  = 1;
uint8_t  current_state          = 0;
uint8_t  previous_state         = 0;

Encoder_Config ENCODER_CONFIG = {
    .clk     = GPIO_PIN_5,
    .ds      = GPIO_PIN_4,
    .btn     = GPIO_PIN_3,
    .onLeft  = &ENC_clr_minus,
    .onRight = &ENC_clr_plus,
    .onClick = &ENC_clr_change,
};

void stm8_init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    LCD_I2C_Init(0x27, 16, 2);
    LCD_update();
    Encoder_Init(&ENCODER_CONFIG);
    micros_init();
    counter_reset();  
    GPIO_Init(RGB_PORT,  RED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(RGB_PORT,  GRN_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);   
    GPIO_Init(RGB_PORT,  BLU_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
}

void RGB_PWM(void)
{
    if (counter[1] > 0)
            {
                ON(RGB_PORT, RED_PIN);
                counter[1] -= 1;

            } else {
                if (counter[1] == 0)
                {
                    OFF(RGB_PORT, RED_PIN);
                }
            }

            if (counter[2] > 0)
            {
                ON(RGB_PORT, GRN_PIN);
                counter[2] -= 1;

            } else {
                if (counter[2] == 0)
                {
                    OFF(RGB_PORT, GRN_PIN);
                }
            }

            if (counter[3] > 0)
            {
                ON(RGB_PORT, BLU_PIN);
                counter[3] -= 1;

            } else {
                if (counter[3] == 0)
                {
                    OFF(RGB_PORT, BLU_PIN);
                }
            }
}

void ENC_clr_change(void)
{
    index += 1;
    if (index > 3)
    {
        index = 1;
    }
}

void ENC_clr_minus(void)
{
    counter_reference[index] -= 2;
    if (counter_reference[index] <= 0)
    {
        counter_reference[index] = 0;
    }
}

void ENC_clr_plus(void)
{
    counter_reference[index] += 2;
    if (counter_reference[index] >= 100)
    {
        counter_reference[index] = 100;
    }
}

void counter_reset(void)
{
    counter[0] = counter_reference[0];
    counter[1] = counter_reference[1];
    counter[2] = counter_reference[2];
    counter[3] = counter_reference[3];
}

void LCD_update(void)
{
    uint16_t string[10];
    sprintf(string, "R%d%%G%d%%B%d%%", counter_reference[1], counter_reference[2], counter_reference[3], 65535);

    // LCD_I2C_Clear();
    LCD_I2C_Home();
    // LCD_I2C_SetCursor(0, 0);
    LCD_I2C_Print(string);    
}


int main(void)
{
    stm8_init();

    while (1)
    {

        if (micros() - master_time >= master_period)
        {

            RGB_PWM();

            counter[0] += 1;
            if (counter[0] > 100)
            {
                cycles += 1;
                if (cycles >= 100)
                {
                    LCD_I2C_Clear();
                    cycles = 0;
                }

                LCD_update();
                counter_reset();
                
            }      
            master_time = micros();
        }
        
    }
}