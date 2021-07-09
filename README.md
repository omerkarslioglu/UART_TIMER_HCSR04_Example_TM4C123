# UART_TIMER_HCSR04_Example_TM4C123
UART &amp; TIMER - HCSR04 Example On TM4C123

# Pin/Port Selection :

I have made my connections as in the image below with little differences.

Since I observed that the HC SR04 sensor does not work properly with +3.3 V, I fed this sensor with +5 V using an external source.

I did not connect the Vcc part because the USB TTL converter is fed from the computer. I just connected the GND line.

I didn't use PF4 as the leds are connected to PF3, PF2 and PF1 in Tiva tm4c123 launch pad.

I connected the TX-RX of the USB-TTL converter to PE4(TX)-PE5(RX).

Since there is no timer peripheral to PA4, I connected the sensor's ECHO pin to PB6 (see Tiva™ C Series TM4C123GH6PM Datasheet on page 650 - Table 10-2. GPIO Pins and Alternate Functions).

![image](https://user-images.githubusercontent.com/67158049/125120171-c6ff5480-e0fa-11eb-8be6-dcae7d2f1bbd.png)

# Result And Discussion

The main purpose of the code I have written and this application I have prepared is to send the value received from the ultrasonic sensor to the computer via UART serial communication.It would be good to start my story about this laboratory application by explaining how I use the HC-SR04 ultrasonic sensor.

The signal applied from the Trig pin of the sensor provides an ultrasonic sound emission at a frequency of 40 kHz.

When this sound wave hits any object and returns to the sensor, the Echo pin becomes active.For time sensitive operations like in this application, we need a code that uses TM4C123's Timer interrupts. If I try to measure the time without using a method such as timer interrupt, the result I will get will not be very efficient.I have configured the TIMER0 peripheral to create a sensitive delay function.

The registers I want to mention in this configuration I set the CFG register to 0x4 for 16-bit conversion. I set the TAMR register to 0x17 to make it work in capture mode. Finally, I enabled the TIMER0 peripheral and finished my configuration.It would be nice if I mention the delayMicroSecond() function after configuration. The important point here is that the counter starts counting with a loop and the increase of each counter depends on the timeout flag being removed. For this, I had the RIS register of the TIMER constantly checked. After the timeout flag was removed, I cleared the timeout flag value thanks to the ICR register.

The distance measurement is performed with the measureDistance() function that I have prepared.The working logic of the measureDistance() function is as follows:

The first trigger pin becomes high, then thanks to the TIMER peripheral in the while loop, the echo pin performs the distance measurement by counting the rising edges during this time.While doing this, the timer capture flag is cleared with the ICR register, when the flag is removed (when the RIS register is equal to 4), the TAR register is kept in a variable. Since the capture flag changes with every logic change, I read the value from the TAR register twice in the same loop (while loop). I returned the value found by subtracting the falling edges from the total changed logic edge state.I processed the measureDistance( ) function in the main function by assigning it to the time variable and calculated the distance. Later, I sent this data to the PC via UART via COM connection.

Now, let me briefly explain how I use the UART serial communication protocol and peripheral on the launch pad.

By short definition, UART is a hardware communication protocol that uses asynchronous serial communication with configurable speed. Asynchronous means there is no clock signal to synchronize the output bits from the transmitting device going to the receiving end.It uses two buses, TX transmitting data and RX receiving data.

Let me now explain how I initialize the UART peripheral.While performing the UART communication, the baudrate (data transport rate) must be set first after the clock configurations connected to the UART peripharel are made.I assigned 104 to the IBRD register and 11 to the FBRD register to set the baudrate to 9600.

Since the data bit length is usually 8 and the frame is set as one stop bit, I have configured it this way.That's why I assigned 0x60 to the LCRH register. Thus, my data length became 8 bits.I declared that with CC register it will work with system clock of UART.Then I activated the UART with the CTL register. Finally ,I determined my tx & rx pins by configuring GPIO AFSEL and GPIO PCTL registers.

UART Functions :

printChar(unsigned char data) : If transmitter bus is available , it sends the data to UART Data register .

printingString(char *str) : If transmitter bus is available , it sends the data to UART Data register as byte byte .

# Project Video Link :
https://youtu.be/RsW6qkvIKTA

Also you can look my other project like the project : https://www.youtube.com/watch?v=MFl8qA0JchM

Thanks for reading .

Ömer Karslıoğlu
