##=================================================================================================
## Name        : Makefile
## Author      : vmoreno
## Date        : 24/12/2021
## Version     : 1.0
## Description : Makefile to generate application.
##=================================================================================================

CC = g++
CFLAGS=-I.
DEPS = SerialPort.h
OBJ = Test_SerialPort.o SerialPort.o
TARGET = SERIAL_PORT

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

.PHONY: clean

clean:
	rm -f ./*.o $(TARGET)
