/* choi++20181201 interrupt test */
  // Write on GPIO48 which can give signal to GPIO50 at every 1 second.
  // Connect GPIO48 pin to GPIO50 so it can give interrupt to GPIO50 at every 1 second.

#include <tinyara/config.h>
#include <apps/shell/tash.h>
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <unistd.h>
#include "iotbus_gpio.h"
#include "iotbus_error.h"
#include "iotapi_evt_handler.h"			

#include <tinyara/gpio.h>

struct _iotbus_gpio_s {
	int pin;
	iotbus_gpio_drive_e drive;
	iotbus_gpio_direction_e dir;
	iotbus_gpio_edge_e edge;
	int fd;
	gpio_isr_cb isr_cb;
void *ud;
};

iotbus_gpio_context_h g_gpio;
iotbus_gpio_context_h g_pin_out;

unsigned int count = 0;
unsigned int seconds = 7200;

int gpio_flag_callback = 0;
int gpio_pin = 50;
int gpio_out = 48;


void gpio_event_callback(void *user_data);
void gpio_register_callback(void);
void gpio_unregister_callback(void);


int interrupt_init(unsigned int c)
{
	count = c;
	g_pin_out = iotbus_gpio_open(gpio_out);
	if(NULL == g_pin_out) {
		printf("Failed iotbus_gpio_open for pin %d\n", gpio_out);
		return -1;
	}
	iotbus_gpio_set_direction(g_pin_out, IOTBUS_GPIO_DIRECTION_OUT);

	gpio_register_callback();

	printf("Interrupt from GPIO 48 to GPIO 50¡¦\n");
	return 0;
}

void interrupt_close()
{
	gpio_unregister_callback();
	iotbus_gpio_close(g_pin_out);

	return ;
}

volatile bool flagg = true;

int interrupt_main()
{

	g_pin_out = iotbus_gpio_open(gpio_out);
	if(NULL == g_pin_out) {
		printf("Failed iotbus_gpio_open for pin %d\n", gpio_out);
		return -1;
	}
	iotbus_gpio_set_direction(g_pin_out, IOTBUS_GPIO_DIRECTION_OUT);

	gpio_register_callback();

	printf("Interrupt from GPIO 48 to GPIO 50¡¦\n");


	if(1 == gpio_flag_callback){
		printf("call back invoked\n");
		gpio_flag_callback = 0;
	}
  // Write on GPIO48 which can give signal to GPIO50 at every 1 second.
  //Connect GPIO48 pin to GPIO50 so it can give interrupt to GPIO50 at every 1 second.
	iotbus_gpio_write(g_pin_out, 1);
	sleep(1);

	return 0;
}

void gpio_event_callback(void *user_data)
{
	/*
	static int count = 0; //remove noise but works once.
	iotbus_gpio_set_direction(g_gpio, IOTBUS_GPIO_DIRECTION_OUT);
	count++;
	printf("Count = %d\n", count);*/

	printf("\ngpio_event_callback called\n");
	iotbus_gpio_context_h hnd = (iotbus_gpio_context_h) user_data;
	int value = iotbus_gpio_read(hnd);
	printf("GPIO Value = %d\n", value);
	gpio_flag_callback = 1;
	if(value==1)
	{
		flagg = !flagg;
		timer_main(seconds, flagg);
	}
	return;
}

void gpio_register_callback(void)
{
	printf("gpio_register_callback called\n");
	gpio_flag_callback = 0;
	int ret;
	int data = 0;

 // Setup event handler
	iotapi_initialize();

	g_gpio = iotbus_gpio_open(gpio_pin);
	if(NULL == g_gpio) {
     	printf("Failed iotbus_gpio_open for pin %d\n", gpio_pin);
     	return;
	}

	iotbus_gpio_set_direction(g_gpio, IOTBUS_GPIO_DIRECTION_IN);

	ret = iotbus_gpio_register_cb(g_gpio, IOTBUS_GPIO_EDGE_RISING, gpio_event_callback, (void *)g_gpio);
	if(0 != ret) {
		printf("Failed iotbus_gpio_register_cb for pin %d\n", gpio_pin);
		iotbus_gpio_close(g_gpio);
		return;
	}
}

void gpio_unregister_callback(void)
{
	printf("gpio_unregister_callback called\n");

	int ret = iotbus_gpio_unregister_cb(g_gpio);
	if(0 != ret) {
		printf("Failed iotbus_gpio_unregister_cb\n");
	}

	iotbus_gpio_close(g_gpio);
}


int main(int argc, FAR char *argv[]){
	tash_cmd_install("interrupt", interrupt_main, TASH_EXECMD_SYNC);
	return 0;
}
