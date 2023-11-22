# GPIO Module for Linux Kernel on a Raspberry Pi 3b

This repository features a straightforward Linux Kernel Module (LKM) for GPIO (General Purpose Input/Output) control. The module is specifically crafted for Raspberry Pi 3b, offering fundamental capabilities to configure and manage GPIO signals. In this instance, Pin 20 on the Raspberry Pi 3b is utilized to control an LED.

Tested on Linux Kernel version 6.1.0-rpi4-rpi-v7.

# Preparation Steps

## 1. Update Package Lists
```bash
sudo apt update
```
## 2. Install Raspberry Pi Kernel Headers
```bash
sudo apt install raspberrypi-kernel-headers
```
## 3. Upgrade Installed Packages
```bash
sudo apt upgrade
```
## 4. Build the Kernel Module

Navigate to the directory containing your .c file and the accompanying Makefile.
Ensure that you are using the same GCC compiler version in your Makefile that was used to compile your kernel.
This ensures compatibility and prevents potential issues during the module build process.
Use the make command to build the .ko file.
Example:
```bash
make
```
## 5. Load the Kernel Module
```bash
sudo insmod gpio_driver.ko
```
## 6. Set Appropriate Permissions
```bash
sudo chmod 666 /dev/my_gpio_driver
```
## 7. Control LED
  Send ON = 1 or OFF = 0 to the driver.
  Example:
```bash
echo 1 > /dev/my_gpio_driver
```
## 8. Unload the Kernel Module
```bash
sudo rmmod gpio_driver.ko
```
## Features

- User-space interface for controlling LED through a character device file.
- Designed for Raspberry Pi 3b.
