#include "stm8s.h"
// #include "encoder.h"
#include "milis.h"
#include "LCD_I2C.h"

#define RGB_PORT    GPIOD
#define RED_PIN     GPIO_PIN_4
#define GRN_PIN     GPIO_PIN_5
#define BLU_PIN     GPIO_PIN_6
#define CTRL_PORT   GPIOA
#define MINUS       GPIO_PIN_1
#define CHANGE      GPIO_PIN_2
#define PLUS        GPIO_PIN_3
#define FLIP        GPIO_WriteReverse
#define OFF         GPIO_WriteLow
#define ON          GPIO_WriteHigh
#define micros_init milis_init
#define micros      milis

void colour_control(void);
void counter_reset(void);

uint32_t master_time            = 0;
uint8_t  master_period          = 1; 
uint8_t  counter[4]             = {0, 0, 0, 0};         // master, R, G, B
int8_t   counter_reference[4]   = {0, 50, 50, 50};      // master, R, G, B
uint8_t  index                  = 1;

// INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
// {
//     if (GPIO_ReadInputPin(CTRL_PORT, CHANGE) == RESET)
//     {
//         index += 1;
//         if (index > 3)
//         {
//             index = 1;
//         }
//     }

//     if (GPIO_ReadInputPin(CTRL_PORT, PLUS) == RESET)
//     {
//         counter_reference[index] += 1;
//         if (counter_reference[index] >= 100)
//         {
//             counter_reference[index] = 100;
//         }
//     }

//     if (GPIO_ReadInputPin(CTRL_PORT, MINUS) == RESET)
//     {
//         counter_reference[index] -= 1;
//         if (counter_reference[index] <= 0)
//         {
//             counter_reference[index] = 0;
//         }
//     }
// }

void colour_control(void)
{
    if (GPIO_ReadInputPin(CTRL_PORT, CHANGE) == RESET)
    {
        index += 1;
        if (index > 3)
        {
            index = 1;
        }
        return;
    }

    if (GPIO_ReadInputPin(CTRL_PORT, PLUS) == RESET)
    {
        counter_reference[index] += 1;
        if (counter_reference[index] >= 100)
        {
            counter_reference[index] = 100;
        }
        return;
    }

    if (GPIO_ReadInputPin(CTRL_PORT, MINUS) == RESET)
    {
        counter_reference[index] -= 1;
        if (counter_reference[index] <= 0)
        {
            counter_reference[index] = 0;
        }
        return;
    }
}

void counter_reset(void)
{
    counter[0] = counter_reference[0];
    counter[1] = counter_reference[1];
    counter[2] = counter_reference[2];
    counter[3] = counter_reference[3];
}

int main(void)
{

    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    enableInterrupts();
    micros_init();

    LCD_I2C_Init(0x27, 16, 2);
    // Encoder_Init(&ENCODER_CONFIG);
    // LCD_I2C_Display();
    // LCD_I2C_SetCursor(0, 1);
    LCD_I2C_Clear();
    LCD_I2C_Print("Hello World");
    // LCD_I2C_NoBacklight();
    GPIO_Init(RGB_PORT,  RED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(RGB_PORT,  GRN_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);   
    GPIO_Init(RGB_PORT,  BLU_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(CTRL_PORT, MINUS,   GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(CTRL_PORT, CHANGE,  GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(CTRL_PORT, PLUS,    GPIO_MODE_IN_PU_NO_IT);

    counter_reset();  

    while (1)
    {

        LCD_I2C_Print("Hello World");

        if (micros() - master_time >= master_period)
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

           
            counter[0] += 1;
            if (counter[0] > 100)
            {
                colour_control();
                counter_reset();
            }      
            master_time = micros();
        }
    }
}