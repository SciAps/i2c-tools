#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

float raw_to_psi(int raw)
{
	return ((float)(raw - 0x0666)*(15-0))/(float)(0x399A-0x0666) + 0;
}

float raw_to_c(int raw)
{
	return (((float)raw / 2047.0) * 200.0)-50;
}

int main(int argc, char*argv[])
{
	int file;
	char filename[40];
	int addr = 0x28;
	char buf[4];
	int bytesRead;
	int bytesLeft;
	int i;
	int value;

	sprintf(filename, "/dev/i2c-3");
	if((file = open(filename,O_RDWR)) < 0) {
		printf("Error: could not open %s\n", filename);
		exit(1);
	}

	if (ioctl(file,I2C_SLAVE,addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }

    memset(buf, 0, sizeof(char)*4);

	bytesLeft = 4;
	i = 0;
    while(bytesLeft > 0) {
    	bytesRead = read(file, &buf[i], bytesLeft);
    	i += bytesRead;
    	bytesLeft -=  bytesRead;
    }

    close(file);

    printf("Read: 0x%02x 0x%02x 0x%02x 0x%02x\n", buf[0], buf[1], buf[2], buf[3]);

	value = (buf[0] << 8) | (buf[1]);
	printf("pressure: %d ==> %f psi\n", value, raw_to_psi(value));

	value = (buf[2] << 3);
	printf("temp: %d ==> %f C\n", value, raw_to_c(value));


    return 0;
}