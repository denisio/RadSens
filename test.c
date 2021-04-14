#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define RS_DEFAULT_I2C_ADDRESS 0x66

// Bus 0 - /dev/i2c-0
// Bus 1 - /dev/i2c-1
//
// gcc test.c -o test

void main()
{
    int      res             = 0;
    int      fd              = -1;
    char    *bus             = "/dev/i2c-1";
    uint8_t  rbuf[19]        = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char     cmd             = 0;
    uint32_t pulse_cnt       = 0;
    float    intensy_static  = 0;
    float    intensy_dyanmic = 0;
    uint8_t  sensor_address  = 0;
    
    if ((fd = open(bus, O_RDWR)) < 0) {
        fprintf(stderr, "Error: failed to open the bus!\n");
        exit(1);
    }

    // Get I2C device
    if (ioctl(fd, I2C_SLAVE, RS_DEFAULT_I2C_ADDRESS) != 0) {
        fprintf(stderr, "Error: failed ioctl!\n");
        close(fd);
        exit(1);
    }

    res = write(fd, &cmd, 1);
    if (res <= 0) {
        fprintf(stderr, "Error: write byte failed!\n");
        close(fd);
        exit(1);
    }

    res = read(fd, &(rbuf[0]), 19);
    if (res <= 0) {
        fprintf(stderr, "Error: read 19 bytes failed!\n");
        close(fd);
        exit(1);
    }
    
    //fprintf(stderr, "len: %d\n",  res);
    //fprintf(stderr, "id:  %d\n",  rbuf[0]);
    //fprintf(stderr, "ver: %d\n",  rbuf[1]);
    
    pulse_cnt       = (rbuf[9] << 8) | rbuf[10];
    intensy_static  = (((uint32_t)rbuf[6] << 16) | ((uint16_t)rbuf[7] << 8) | rbuf[8]) / 10.0;
    intensy_dyanmic = (((uint32_t)rbuf[3] << 16) | ((uint16_t)rbuf[4] << 8) | rbuf[5]) / 10.0;
     
    fprintf(stderr, "cnt: %d\n",     pulse_cnt);
    fprintf(stderr, "static: %f\n",  intensy_static);
    fprintf(stderr, "dynamic: %f\n", intensy_dyanmic);
    
    //fprintf(stderr, "addr: %d\n", rbuf[16]);
    //fprintf(stderr, "sens: %d\n", rbuf[18]);

    close(fd);
}
