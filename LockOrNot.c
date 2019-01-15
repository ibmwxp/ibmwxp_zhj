#include "gpiolib_addr.h"
#include "gpiolib_reg.h"
#include <ctype.h>
#include <string.h>


#include <stdlib.h>



#include <stdint.h>
#include <stdio.h> //for the printf() function
//needed for the watchdog specific constants
#include <unistd.h>			//needed for sleep
#include <sys/ioctl.h>		//needed for the ioctl function




int main(void)
{
    GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}

	//Set pin 17 as an output pin, same as what was done in the blink program
	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));
	sel_reg |= 1  << 21;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);

    char password[105] = "Confirmed";
    char audio[200], openclose[100];
    FILE *b;
    b = fopen("Audio.txt", "r");
    fscanf(b, "%s%s", &audio, &openclose);
	printf("%s %s", audio, openclose);


    if(strcmp(audio, password) == 0){
        if(strcmp(openclose, "Open") == 0){

            for (int a = 0; a< 2; a++) {
                //Turn on the LED by changing the 24th bit in the set register to 1
                gpiolib_write_reg(gpio, GPSET(0), 1 << 17);
                //Pause the program for half a second
                usleep(250000);
                //Turn the LED off by setting the 24th bit in the clear register to 1
                gpiolib_write_reg(gpio, GPCLR(0), 1 << 17);
                //Pause the program for half a second
                usleep(250000);
            }
        }
        else {

            for (int a = 0; a< 3; a++) {
                //Turn on the LED by changing the 24th bit in the set register to 1
                gpiolib_write_reg(gpio, GPSET(0), 1 << 17);
                //Pause the program for half a second
                usleep(250000);
                //Turn the LED off by setting the 24th bit in the clear register to 1
                gpiolib_write_reg(gpio, GPCLR(0), 1 << 17);
                //Pause the program for half a second
                usleep(250000);
            }
        }
    } 

    	

   gpiolib_free_gpio(gpio);

}