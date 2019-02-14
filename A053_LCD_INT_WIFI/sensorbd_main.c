/****************************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include <tinyara/analog/adc.h>
#include <tinyara/analog/ioctl.h>

#include <stdio.h>
#include <apps/shell/tash.h>
#include <string.h>
#include <fcntl.h>
#include <tinyara/gpio.h>
#include "sh1106.h"


static int gpio_read(int port)
{
   char buf[4];
   char devpath[16];
   snprintf(devpath, 16, "/dev/gpio%d", port);
   int fd = open(devpath, O_RDWR);

   ioctl(fd, GPIOIOC_SET_DIRECTION, GPIO_DIRECTION_IN);
   if (read(fd, buf, sizeof(buf)) < 0) {
      printf("read error\n");
      return -1;
   }
   close(fd);

   return buf[0] == '1';
}

static void show_usage(FAR const char *program)
{
	printf("USAGE:\n");
	printf(" %s led        : gpio led on/off test\n", program);
	printf(" %s ledpwm     : pwm led on/off test\n", program);
	printf(" %s ledst      : gpio starter board led test\n", program);
	printf(" %s uart       : uart loopback test\n", program);
	printf(" %s gpio       : gpio loopback test\n", program);
	printf(" %s button     : gpio button interrupt test\n", program);
	printf(" %s pwm        : pwm buzzer test\n", program);
	printf(" %s tcs34725   : i2c rgb sensor(tcs34725) test\n", program);
	printf(" %s mpu9250    : i2c 9-axis motion tracking sensor(MPU9250) test\n", program);
	printf(" %s k6ds3      : spi 6-axis acceler sensor(K6DS3) test\n", program);
	printf(" %s lis3lv02qd : spi 3-axis acceler sensor(LIS3LV02QD) test\n", program);
	printf(" %s adctest    : adc print all channel value\n", program);
	printf(" %s alc5658 loopback   : i2s audio codec(alc5658) loopback\n", program);
	printf(" %s alc5658 record     : i2s audio codec(alc5658) record\n", program);
}


/*
 * sh1106.c
 *
 *  Created on: 2018. 9. 29.
 *      Author: nwerl
 */

extern volatile bool flagg;
extern volatile unsigned int seconds;


#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else

static int digitalRead(int port)
{
	char buf[4];
	char devpath[16];
	snprintf(devpath, 16, "/dev/gpio%d", port);
	int fd = open(devpath, O_RDWR);
	ioctl(fd, GPIOIOC_SET_DIRECTION, GPIO_DIRECTION_IN);
	read(fd, buf, sizeof(buf));
	close(fd);
	return buf[0] == '1';
}



int sensorbd_main(int argc, FAR char *argv[])
#endif
{
	int j=1;
	bool _52 = false;
	bool _53 = false;
	bool _54 = false;
	wifi_init();
	timer_init();
	interrupt_main();
	while(1)
	{
		if(flagg && seconds>0)
		{
			timer_main(seconds--, flagg);
			wifi_publish();
			sleep(1);
		}
		else
		{
			if(digitalRead(52))
			{
				_52 = true;
			}
			else
			{
				if(_52)
				{
					seconds += 180;
					printf("%d", seconds);
					timer_main(seconds, flagg);
				}
				_52 = false;
			}
			if(digitalRead(53))
			{
				_53 = true;
			}
			else
			{
				if(_53)
				{
					seconds -= 180;
					printf("%d", seconds);
					timer_main(seconds, flagg);
				}
				_53 = false;
			}if(digitalRead(54))
			{
				_54 = true;
			}
			else
			{
				if(_54)
				{
					seconds = 0;
					printf("%d", seconds);
					timer_main(seconds, flagg);
				}
				_54 = false;
			}
			wifi_publish();
			up_mdelay(100);
		}
	}
	interrupt_close();
}
