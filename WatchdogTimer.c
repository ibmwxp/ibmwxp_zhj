#include "gpiolib_addr.h"
#include "gpiolib_reg.h"

#include <stdint.h>
#include <stdio.h>		//for the printf() function
#include <fcntl.h>
#include <linux/watchdog.h> 	//needed for the watchdog specific constants
#include <unistd.h> 		//needed for sleep
#include <sys/ioctl.h> 		//needed for the ioctl function
#include <stdlib.h> 		//for atoi
#include <time.h> 		//for time_t and the time() function
#include <sys/time.h>           //for gettimeofday()

//Below is a macro that had been defined to output appropriate logging messages
//You can think of it as being similar to a function
//file        - will be the file pointer to the log file
//time        - will be the current time at which the message is being printed
//programName - will be the name of the program, in this case it will be Lab4Sample
//str         - will be a string that contains the message that will be printed to the file.
#define PRINT_MSG(file, time, programName, str) \
	do{ \
			fprintf(file, "%s : %s : %s", time, programName, str); \
			fflush(file); \
	}while(0)
		
	
#define PRINT_Results(file, time, programName, laser1Count,laser2Count, numberIn, numberOut) \
	do{ \
			fprintf(file, "%s : %s", time, programName); \
			fprintf(file, "Laser 1 was broken %d times \n", laser1Count); \
			fprintf(file, "Laser 2 was broken %d times \n", laser2Count);\
			fprintf(file,"%d objects entered the room \n", numberIn); \
			fprintf(file, "%d objects exitted the room \n", numberOut);  \
			fflush(file); \
	}while(0)




/* You may want to create helper functions for the Hardware Dependent functions*/

//This function should initialize the GPIO pins
GPIO_Handle initializeGPIO(){
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL){
		perror("Could not initialize GPIO");
	}
	return gpio;
}


//This function should accept the diode number (1 or 2) and output
//a 0 if the laser beam is not reaching the diode, a 1 if the laser
//beam is reaching the diode or -1 if an error occurs.
#define LASER1_PIN_NUM 4
#define LASER2_PIN_NUM 6
#define PROGRAMNAME "watchdog"
int laserDiodeStatus(GPIO_Handle gpio, int diodeNumber){
	if(gpio == NULL){
		return -1;
	}

	if(diodeNumber == 1){
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));

		if(level_reg & (1 << LASER1_PIN_NUM)){
			return 1;
		}
		else{
			return 0;
		}
	}
	else if(diodeNumber == 2){
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));

		if(level_reg & (1 << LASER2_PIN_NUM)){
			return 1;
		}
		else{
			return 0;
		}
	}

}


//END OF HARDWARE DEPENDENT CODE

//This function will output the number of times each laser was broken
//and it will output how many objects have moved into and out of the room.

//laser1Count will be how many times laser 1 is broken (the left laser).
//laser2Count will be how many times laser 2 is broken (the right laser).
//numberIn will be the number  of objects that moved into the room.
//numberOut will be the number of objects that moved out of the room.
void outputMessage(int laser1Count, int laser2Count, int numberIn, int numberOut){
	printf("Laser 1 was broken %d times \n", laser1Count);
	printf("Laser 2 was broken %d times \n", laser2Count);
	printf("%d objects entered the room \n", numberIn);
	printf("%d objects exitted the room \n", numberOut);
}

//This function accepts an errorCode. You can define what the corresponding error code
//will be for each type of error that may occur.
void errorMessage(int errorCode){
	fprintf(stderr, "An error occured; the error code was %d \n", errorCode);
}





void readConfig(FILE* configFile, int* timeout, char* logFileName, char* results)
{
	int i = 0;
	
	//A char array to act as a buffer for the file
	char buffer[500];

	//The value of the timeout variable is set to zero at the start
	*timeout = 0;
	

	//This is a variable used to track which input we are currently looking
	//for (timeout, logFileName or numBlinks)
	//This will 
	//fgets(buffer, 255, configFile);
	//This will check that the file can still be read from and if it can,
	//then the loop will check to see if the line may have any useful 
	//information.
	while(fgets(buffer, 500, configFile) != NULL) {
				
		int i = 0;
		int j = 0;
		int k = 0;
		int l = 0;
		
		while (buffer[i] != '#' || buffer[i] != ' ' || buffer[i] != 0) {
			if((buffer[i] == 'W') && (buffer[i+1] == 'A') && (buffer[i+2] == 'T') &&(buffer[i+3] == 'C')
				&&(buffer[i+4] == 'H') &&(buffer[i+5] == 'D') &&(buffer[i+6] == 'O') &&(buffer[i+7] == 'G')) {
				
				i += 16;					
				while(buffer[i] != 0) {
					if (buffer[i] == '=') { 
						i++;
						while(buffer[i] != 0) {
						//If the character is a number from 0 to 9
							if(buffer[i] >= '0' && buffer[i] <= '9') {
						
							//Move the previous digits up one position and add the
							//new digit
								*timeout = (*timeout *10) + (buffer[i] - '0');
								j = 1;
							}
							i++;
						}
					}
				}
				i = 0;
			}
						
			else if((buffer[i] == 'L') && (buffer[i+1] == 'O') && (buffer[i+2] == 'G') &&(buffer[i+3] == 'F')
				&&(buffer[i+4] == 'I') &&(buffer[i+5] == 'L') &&(buffer[i+6] == 'E')) {
				
				i += 7;					
				while(buffer[i] != 0) {
					if (buffer[i] == '=') {
						i++;
						//Loop runs while the character is not a newline or null
						while(buffer[i] != 0  && buffer[i] != '\n')
						{
							//If the characters after the equal sign are not spaces or
							//equal signs, then it will add that character to the string
							if(buffer[i] != ' ' && buffer[i] != '=')
							{
								logFileName[k] = buffer[i];
								j++;
							}
							i++;
						}
						//Add a null terminator at the end
						logFileName[k] = 0;
					}	
				}
				i=0;
			}
			else if((buffer[i] == 'r') && (buffer[i+1] == 'e') && (buffer[i+2] == 's') &&(buffer[i+3] == 'u')
				&&(buffer[i+4] == 'l') &&(buffer[i+5] == 't') &&(buffer[i+6] == 's')) {
				
				i += 7;					
				while(buffer[i] != 0) {
					if (buffer[i] == '=') {
						i++;						//Loop runs while the character is not a newline or null
						while(buffer[i] != 0  && buffer[i] != '\n')
						{
							//If the characters after the equal sign are not spaces or
							//equal signs, then it will add that character to the string
							if(buffer[i] != ' ' && buffer[i] != '=')
							{
								results[l] = buffer[i];
								l++;
							}
							i++;
						}
						//Add a null terminator at the end
						results[l] = 0;
					}	
				}
				i = 0;
			}
		}
	}
	if (*timeout <= 0 || *timeout > 15) {
		*timeout = 15;
	}
	//check if timeout has value or anyof them has value
}


//This function will get the current time using the gettimeofday function
void getTime(char* buffer)
{
	//Create a timeval struct named tv
  	struct timeval tv;

	//Create a time_t variable named curtime
  	time_t curtime;


	//Get the current time and store it in the tv struct
  	gettimeofday(&tv, NULL); 

	//Set curtime to be equal to the number of seconds in tv
  	curtime=tv.tv_sec;

	//This will set buffer to be equal to a string that in
	//equivalent to the current date, in a month, day, year and
	//the current time in 24 hour notation.
  	strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));

} 






int main(const int argc, const char* const argv[]){
	enum State { START, LEFT_IN, LEFT_BOTH, LEFT_LEAVE,RIGHT_OUT, RIGHT_BOTH, RIGHT_LEAVE};
	enum State s = START;
	int laser1Count = 0;
	int laser2Count = 0;
	int numberIn = 0;
	int numberOut = 0;
	FILE* configFile;
	configFile = fopen("/home/pi/Lab4.cfg", "r");
	if(!configFile)
	{
		configFile = fopen("/home/pi/DefaultConfig.cfg", "r");
	}
	int timeout;
	char logFileName[50];
	char results[50];
	readConfig(configFile, &timeout, logFileName, results);   ///// set default file to write to if cant open
	fclose(configFile);
	FILE* logFile;
	logFile = fopen(logFileName, "a");
	if(!logFile)
	{
		logFile = fopen("/home/pi/LogDefault.log", "a");
	}
	FILE* Stats;
	Stats = fopen(results, "a");
	if(!Stats)
	{
		Stats = fopen("/home/pi/StatsDefault.log", "a");
	}
	///results file same thing
	char times[30];
	getTime(times);
	GPIO_Handle gpio = initializeGPIO();
	getTime(times);
	PRINT_MSG(logFile, time, PROGRAMNAME, "The GPIO pins have been initialized\n\n");
	
	uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));
	sel_reg &= ~(1 << 18);
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);
	uint32_t sel_reg1 = gpiolib_read_reg(gpio, GPFSEL(1));
	sel_reg1 &= ~(1 << 18);
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg1);
	PRINT_MSG(logFile, times,  PROGRAMNAME, "Pin 6 has been set to output\n\n");
	int watchdog;
	if ((watchdog = open("/dev/watchdog", O_RDWR | O_NOCTTY)) < 0) {
		PRINT_MSG(logFile, times,  PROGRAMNAME, "The Watchdog cannot open Watchdog");
	}
	
	ioctl(watchdog, WDIOC_SETTIMEOUT, &timeout);
	getTime(times);
	//Log that the Watchdog time limit has been set
	PRINT_MSG(logFile, times,  PROGRAMNAME, "The Watchdog time limit has been set\n\n");
	ioctl(watchdog, WDIOC_GETTIMEOUT, &timeout);	
	printf("The watchdog timeout is %d seconds.\n\n", timeout);  ///print to logfile 

	

		
	if(argc != 2){
		errorMessage(-1);
		return -1;
	}


	time_t startTime = time(NULL);


	while(1){ 
	
		int left = laserDiodeStatus(gpio, 1);//Set laser1 on the left
		int right = laserDiodeStatus(gpio, 2);//Set laser2 on the right
		fprintf(stderr, "status %i %i\n ", left, right);


		switch (s){
			case START:
				
				//0 means light not received, 1 means light recieved
				if(left == 0 && right == 1){ // laser1 is blocked from  left side
					++laser1Count;
					s = LEFT_IN;
					break;
				}else if(left == 1 && right == 0){ // laser2 is blocked from right side
					++laser2Count;
					s = RIGHT_OUT;
					break;
				}
				else if(left == 0 && right == 0) {
					errorMessage(-1);
					return -1;
				}
				break;
				
			case LEFT_IN:
				if(left == 0 && right == 0){//both laser1 and laser2 are blcoked
					s = LEFT_BOTH;
					++laser2Count;
					break;
				}else if(left == 1 && right == 1){
					s = START;
					break;
				}
				break;
			case LEFT_BOTH:
				if(left == 1 && right == 0){
					s = LEFT_LEAVE;
					break;
				}else if(left == 0 && right == 1){
					s = LEFT_IN;
					break;
				}else if(left == 0 && right == 0){
					s = LEFT_BOTH;
					break;
				}
				break;
			case LEFT_LEAVE:
				if(left == 0 && right == 0){
					s = LEFT_BOTH;
					laser1Count++;
					break;
				}else if(left == 1 && right == 1){
					s = START;
					numberIn++;
					break;
				}
				break;

			case RIGHT_OUT:
				if(left == 0 && right == 0){
					s = RIGHT_BOTH;
					++laser1Count;
					break;
				}else if(left == 1 && right == 1){ //both laser1 and laser2 are not blcoked
					s = START;
					break;
				}else if(left == 1 && right == 0){ //laser1 is still being blocked
					s=RIGHT_OUT;
					break;
				}
				break;
			case RIGHT_BOTH:
				if(left == 0 && right == 1){
					s = RIGHT_LEAVE;
					break;
				}else if(left == 1 && right == 0){
					s = RIGHT_OUT;
					break;
				}
				else if (left == 0 && right == 0) {
					s = RIGHT_BOTH;
					break;
				}
				break;
			case RIGHT_LEAVE:
				if(left == 0 && right == 0){
					s = RIGHT_BOTH;
					laser2Count++;
					break;
				}else if(left == 1 && right == 1){
					s = START;
					numberOut++;
					break;
				}
				break;
			default:
				errorMessage(-1);
				return -1;
				break;
		}
		
		if ((time(NULL) - startTime) == 12) {
			PRINT_Results(Stats, times,  PROGRAMNAME, laser1Count,laser2Count, numberIn, numberOut);
		}
		ioctl(watchdog, WDIOC_KEEPALIVE, 0);
		getTime(times);
		//Log that the Watchdog was kicked
		PRINT_MSG(logFile, times,  PROGRAMNAME, "The Watchdog was kicked\n\n");
	} 
	
	return 0;
}

