#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <math.h>

#define I2C_DEVICE "/dev/i2c-1"
#define HMC5883L_ADDR 0x1E
#define HMC5883L_REG_CONFIG_A 0x00
#define HMC5883L_REG_MODE 0x02
#define HMC5883L_REG_DATA_X_MSB 0x03
#define HMC5883L_REG_CONFIG_B 0x01

#define PI 3.14159265359

int main() {
    int i2c_fd;
    char buf[6];

    // Open the I2C bus
    i2c_fd = open(I2C_DEVICE, O_RDWR);
    if (i2c_fd < 0) {
        perror("Failed to open the I2C bus");
        return 1;
    }

    // Set the I2C device address
    if (ioctl(i2c_fd, I2C_SLAVE, HMC5883L_ADDR) < 0) {
        perror("Failed to set I2C device address");
        return 1;
    }

    // Configure the HMC5883L in continuous measurement mode
    buf[0] = HMC5883L_REG_CONFIG_A;
    buf[1] = 0x70; // Set data output rate to 15Hz and normal measurement range
    if (write(i2c_fd, buf, 2) != 2) {
        perror("Failed to configure HMC5883L");
        return 1;
    }

    // Configure the HMC5883L >> gain
    buf[0] = HMC5883L_REG_CONFIG_B;
    buf[1] = 0xa0; // to Configuration Register B for gain
    if (write(i2c_fd, buf, 2) != 2) {
        perror("Failed to configure HMC5883L");
        return 1;
    }

    // Set the HMC5883L to continuous-measurement mode
    buf[0] = HMC5883L_REG_MODE;
    buf[1] = 0x00; // Set mode to continuous measurement
    if (write(i2c_fd, buf, 2) != 2) {
        perror("Failed to set HMC5883L measurement mode");
        return 1;
    }
    double heading=0;

    // Read and print the magnetometer data continuously
    while (1) {
        // Wait for measurement completion
        usleep(6000); // Delay for 6ms (15Hz data output rate)

        // Read the magnetometer data
        buf[0] = HMC5883L_REG_DATA_X_MSB;
        if (write(i2c_fd, buf, 1) != 1) {
            perror("Failed to write register address");
            return 1;
        }

        if (read(i2c_fd, buf, 6) != 6) {
            perror("Failed to read magnetometer data");
            return 1;
        }

        // Combine the MSB and LSB to get the magnetometer values for each axis
        int x = (buf[0] << 8) | buf[1];
        int z = (buf[2] << 8) | buf[3];
        int y = (buf[4] << 8) | buf[5];

        if(x > 32768) x -= 65536;
        if(y > 32768) y -= 65536;
        if(z > 32768) z -= 65536;
        // Calculate the heading in degrees
        heading = atan2(y, x) + 0.22;

        if (heading < 0.0) {
            heading += 2*PI;
        }
        else if (heading > 2*PI) {
            heading -= 2*PI;
        }

        // Print the direction in degrees
        printf("Direction: %.2f degrees\n", (heading * 180/PI));
    }

    // Close the I2C bus
    close(i2c_fd);
    return 0;
}