#include "gpiolib_addr.h"
#include "gpiolib_reg.h"


#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

//Function for Setting pin 17 as an output pin, same as what was done in the blink program using parameters of register and pin
	
void InitializerGPIO(int reg, int pin) {
	//Initialize the GPIO pins and check if initialize correctly
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}

	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(reg));
	sel_reg |= 1  <<  pin;
	gpiolib_write_reg(gpio, GPFSEL(reg), sel_reg);
}
	
//Function for counting the amount of times the button was pressed
int TimesPressed(int timer, int previous, int pin_state, int counter) {

	//Initialize the GPIO pins and check if initialize correctly
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}
	//Timer for 10 seconds
	while(timer < 10000000) {
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));			
		pin_state = level_reg & (1 << 4);

		//if the pin was pressed and released (one complete cycle), increase counter
		if (pin_state && !previous) {
			counter += 1;
		}
		//increment timer by the time the program stops until 10 seconds
		previous = pin_state;
		usleep(1000);
		timer += 1000;
		
	}	

	return counter;
}

//Function for flashing the second led blinker for the amount of times it was pressed previously 
void SecondBlinker(int TimesItBlink, int counter) {
	
	//Initialize the GPIO pins and check if initialize correctly
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}
	//Flash exactly 'counter' many times
	while(TimesItBlink < counter) {
		//Turn on the LED by changing the 24th bit in the set register to 1
		gpiolib_write_reg(gpio, GPSET(0), 1 << 24);
		

		//Pause the program for half a second
		usleep(250000);

		//Turn the LED off by setting the 24th bit in the clear register to 1
		gpiolib_write_reg(gpio, GPCLR(0), 1 << 24);

		//Pause the program for half a second
		usleep(250000);

		++TimesItBlink;

	}
}

int main(void) {

	//Initialize the GPIO pins and check if initialize correctly
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}

	//Function sets pin 17 as an output pin, same as what was done in the blink program
	
	InitializerGPIO(1, 21);

	//Master loop 

	while(1) {
		
		//Declare a variable called level_reg of 32-bit size and assigned the value of the level register
			
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));
			
		//Declare a variable called pin_state and assign the value of bit 4 from the level_reg
	
		int pin_state = level_reg & (1 << 4);

		//turn on light and wait for user to do something
		
		gpiolib_write_reg(gpio, GPSET(0), 1 << 17);
				
		

		//Read the button....If it's pressed, turn off the light and continue ...if not, then keep the light on and wait for user to push button
	
		while (pin_state) {

			level_reg = gpiolib_read_reg(gpio, GPLEV(0));			
			pin_state = level_reg & (1 << 4);
		}

		usleep(100000);

		while (!pin_state) {
			
			level_reg = gpiolib_read_reg(gpio, GPLEV(0));			
			pin_state = level_reg & (1 << 4);
		}

		gpiolib_write_reg(gpio, GPCLR(0), 1 << 17);
		
		//Intitialize counterr to count how many presses of button and other parameters 
		int counterr = 0;
		int previous = 1;
		int timer = 0;
		pin_state = 0;


		//Function counts the number of times the button is pressed within a 10 second interval.
		
		int counter = TimesPressed(timer, previous, pin_state, counterr);
		

		//Function sets pin 24 as an output pin, same as what was done in the blink program
		
		InitializerGPIO(2,12);
			
		//Declare variable as a counter for the number of times it will blink compared to counter
		int TimesItBlink = 0; 


		//Function Loops to blink the LED "counter" times
		
		SecondBlinker(TimesItBlink, counter);

		//Sleep program for 5 seconds ... the 5 second wait
		usleep(5000000);
	}	

	return 0;

}

