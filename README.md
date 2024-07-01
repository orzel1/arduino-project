# arduino-project
Disclaimer: This project was made during my embedded systems course at uni.

As an aim of my embedded systems project, I've constructed a 32 bit calculator which operates on positive integers. As an unique feature this calculor can - if user wishes it - convert the decimal result into binary and hexadecimal result.

[Video preview of the device - Google Drive link](https://1drv.ms/v/c/fc4ff4f0975ccd38/EdAT5jrgN19Hpe1rG4aYv5UBvqlORmu0EhuPB-Lb51SVxg?e=Pi6kqO)

Technical details are provided below.

# Build components

- Arduino Nano v3.0 atmega 328,
- LCD Screen 16x2,
- Three 220 ohm resistors,
- Three LEDs (colored: green, yellow, red),
- Three push buttons
- Piezoelectric speaker
- Potentiometer
  
Compiled and written in Arduino IDE.


# Visual preview
![visual_preview](https://github.com/orzel1/arduino-project/assets/109137110/8856341a-2091-4893-a830-ae17621ab29d)

# Electric scheme
![scheme](https://github.com/orzel1/arduino-project/assets/109137110/7837c6ae-7b01-40f3-8fd5-0224525ec56d)

# Technical details - instruction of use
As mentioned in the introduction - this is a 32 bit calculator which means that it can propely solve and display the result until it doesn't exceeds the number in range between *-2,147,483,648* to *2,147,483,647.*

### a) Possible operations
- Addition,
- Substraction,
- Division,
- Multiplication.

### b) Data validation
The user can input data into the calculator by using three components:
- potentiometer,
- Button B1,
- Button B2.

The use of potentiometer is needed to choose between positive intigers (0-9) and type of operation (+*/-). There is a certain value treshold that - if chosen - lets the user display a disired number or operation.

Button B1 acts as a confirmation. If the user is satisfied with chosen number, pusing the button will result in storing the current number in the memory of the microcontroller.

Button B2 performs a role of separator between factors - by pushing it the user confirms that the first factor operation is completed. Button B2 forces the user to input an operation type (+*/-).

### c) Error Handling
Despite fully controlling the input provided by the user there are three problems that may occur:

- The possibility of overflow,
- Dividing by zero,
- The possibility of exceeding input values beyond the screen.


**Solution for problem no.1**

Detecting the overflow will result in the yellow LED switching ON. The user will be provided with text information on the LCD screen in regards to overflow error and two quick high-pitched beeps from piezoelectric speaker will occur. To retry user will need to press button B3 which will result in resetting the device.

**Solution for problem no.2**

Detecting dividing by zero will result in the red LED switching ON. The user will be provided with text information on the LCD screen in regards to division by zero error and three quick high-pitched beeps from piezoelectric speaker will occur. To retry user will need to press button B3 which will result in resetting the device.

**Solution for problem no.3**

Position of currently inputted data is constantly monitored. This leads to prediction of when the user is about to exceed the screen beyond the visible input area.

**Case of no errors**

If any type of error had't been detected - the device will inform the user that it ended the calculations by switching On the green LED, providing the result on the LCD screen and making one quick high-pitched beep from piezolectric speaker. To make new calculations - the user will need to press button B3 which will result in resetting the device.

# Current plans on improving the device
