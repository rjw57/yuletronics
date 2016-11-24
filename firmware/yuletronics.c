#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LED1 GPIO0
#define LED2 GPIO1
#define LED3 GPIO2
#define LED4 GPIO3
#define LED5 GPIO4
#define LED6 GPIO6
#define LED7 GPIO7
#define LED8 GPIO1

#define LED1_PORT GPIOA
#define LED2_PORT GPIOA
#define LED3_PORT GPIOA
#define LED4_PORT GPIOA
#define LED5_PORT GPIOA
#define LED6_PORT GPIOA
#define LED7_PORT GPIOA
#define LED8_PORT GPIOB

static const uint16_t led_pins[] = {
    0,
    LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8
};

static const uint32_t led_ports[] = {
    0,
    LED1_PORT, LED2_PORT, LED3_PORT, LED4_PORT,
    LED5_PORT, LED6_PORT, LED7_PORT, LED8_PORT
};

#define TURNON(led) gpio_set(led_ports[led], led_pins[led])
#define TURNOFF(led) gpio_clear(led_ports[led], led_pins[led])

static void gpio_setup(void)
{
    int led;

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    for(led=1; led<=8; led++) {
        gpio_clear(led_ports[led], led_pins[led]);
        gpio_mode_setup(
            led_ports[led], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, led_pins[led]);
        gpio_set_output_options(
            led_ports[led], GPIO_OTYPE_PP, GPIO_OSPEED_LOW, led_pins[led]);
    }
}

static void wait(void)
{
    volatile int i;
    for(i=0; i<100000; i++);
}

int main(void)
{
    int brightneses[8];
    int velocities[8];
    int accels[8];

    gpio_setup();

    for(int i=0; i<8; ++i) {
        brightneses[i] = 0;
        velocities[i] = 0;
        accels[i] = 0;
    }

    uint32_t tick;
    while(true)
    {
        ++tick;

        for(int i=0; i<256; ++i) {
            for(int j=0; j<8; ++j) {
                if((brightneses[j]>>8) > i) {
                    TURNON(j+1);
                } else {
                    TURNOFF(j+1);
                }
            }
        }

        for(int i=0; i<8; ++i) {
            int delta = 40000 - brightneses[i];
            int scaling = rand() & 0xff;
            if(delta > 0) {
                accels[i] = (scaling * delta) >> 14;
            } else {
                accels[i] = -((scaling * (-delta)) >> 14);
            }
        }

        for(int i=0; i<8; ++i) {
            int x = velocities[i] + accels[i];
            if(rand() & 1) {
                x += rand() & 0xff;
            } else {
                x -= rand() & 0xff;
            }
            velocities[i] = x;
        }

        for(int i=0; i<8; ++i) {
            int y = brightneses[i] + velocities[i];
            if(y < 1000) {
                y = 1000;
            }
            if(y > 64535) {
                y = 64535;
            }
            brightneses[i] = y;
        }
    }
    return 0;
}
