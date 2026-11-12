#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pigpio.h>

#define LED_PIN 17
#define EXIT_CODE -999

int main(void){

if (gpioInitialise() < 0) {
	fprintf(stderr, "pigpio intialization failed\n");
	return 1;
}

gpioSetMode(LED_PIN, PI_OUTPUT);
gpioWrite(LED_PIN, 0);

float min_temp = 68.0;
float max_temp = 76.0;
char choice;

printf("--- Smart Classroom Comfort Moitor Setup ---\n");
printf("The default acceptable range is %.1f F to %.1f F.\n", min_temp, max_temp);
printf("Would you like to change the acceptable temperature range? (y/n): ");
scanf("%c", &choice);

if (choice == 'y' || choice == 'Y'){
printf("Enter minimum acceptable temperature (F): ");
scanf("%f", &min_temp);
printf("Enter maximum acceptable temperature (F): ");
scanf("%f", &max_temp);
}

if (min_temp > max_temp){
float temp = min_temp;
min_temp = max_temp;
max_temp = temp;
printf("Note: Min and Max were swapped so the range is valid.\n");
printf("New acceptable range set to: %.1f F - %1f F\n\n", min_temp, max_temp);
}

FILE *log_file = fopen("temperature_log.txt", "w");
if (log_file == NULL){
fprintf(stderr, "Error opening log file!\n");
gpioTerminate();
return 1;
}
fprintf(log_file, "Temperature(F), Status, LED\n");

float current_temp;
float total_temp = 0.0;
int count = 0;

printf("\n--- Data Entry Mode ---\n");
printf("Enter temperature readings in Fahrenheit. -999 to stop\n");

while(1){
printf("Enter Reading: ");
if (scanf("%f", &current_temp) != 1){
printf("Invaild Input. Please enter a number.\n");
while (getchar() != '\n');
continue;
}

if (current_temp == EXIT_CODE){
break;
}

const char *status;
const char *led_state;

if (current_temp >= min_temp && current_temp <= max_temp){
status = "NORMAL";
led_state = "OFF";
gpioWrite(LED_PIN, 0);
}

else{
status = "WARNING";
led_state = "ON";
gpioWrite(LED_PIN, 1);
}



printf("%.1f F\t\t%-15s %-10s\n", current_temp, status, led_state);
fprintf(log_file, "%.1f,%s,%s\n",current_temp, status, led_state);

total_temp += current_temp;
count++;
}

fclose(log_file);
gpioWrite(LED_PIN, 0);
gpioTerminate();

if (count > 0){
float avg = total_temp/count;
printf("Total readings recorded: %d\n", count);
printf("Average temperature: %.2f F\n", avg);
}
else{
printf("No data was recorded");
}
return 0;
}
