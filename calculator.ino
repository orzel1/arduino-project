#include <LiquidCrystal_I2C.h>
#include <math.h>
#include <limits.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x3F

const String intro1 = "## KALKULATOR ##";
const String intro2 = "#### [+-*/] ####";
const String Message1 = "Konwersja wyniku";
const String Message2 = "na BIN i HEX";
const String continueMessage1 = "Nacisnij guzik";
const String continueMessage2 = "i zacznij liczyc";
const String divideByZero1 = "BLAD:DZIELENIE";
const String divideByZero2 = "PRZEZ 0!";
const String NumberTooBig1 = "Wynik jest za";
const String NumberTooBig2 = "duzy!";

// coordinate shifts
const int coordinateX = 0;
const int coordinateY = 0;
int shiftX = 1;
int saveShiftX_value = 0; // used to store coordinate X shift when the user would go beyond the screen

int factor1[12];
int factor2[12];
long convertedFactor[2];

char operation = ' ';
long result = 0;
int resultLength = 0;

bool wasUserGreeted = false;
bool divideByZero = false;
bool LED_overflow = false;
bool wholeFactorFilled = false;

// Buttons
const int button_confirm = 2;
const int button_place_operation = 3;
const int button_restart = 4;

// LEDs
const int greenLED = 5;
const int yellowLED = 6;
const int redLED = 7;

const int buzzer = 8;         // Speaker
const int potentiometer = A0; // Potentiometer

void setup()
{
    // LCD
    lcd.begin(16, 2);
    lcd.init();
    lcd.backlight();

    // Inputs
    pinMode(button_confirm, INPUT_PULLUP);
    pinMode(button_place_operation, INPUT_PULLUP);
    pinMode(button_restart, INPUT_PULLUP);

    // Outputs
    pinMode(greenLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    pinMode(buzzer, OUTPUT);
}

void loop()
{
    Serial.begin(115200);

    if (wasUserGreeted == false)
    {
        digitalWrite(greenLED, HIGH);
        displayMessage(intro1, coordinateX, coordinateY, 0);
        displayMessage(intro2, coordinateX, coordinateY + 1, 1500);
        clearScreen();

        displayMessage(Message1, coordinateX, coordinateY, 1000);
        digitalWrite(yellowLED, HIGH);
        displayMessage(Message2, coordinateX + 2, coordinateY + 1, 1000);
        clearScreen();

        displayMessage(continueMessage1, coordinateX + 1, coordinateY, 1000);
        digitalWrite(redLED, HIGH);
        displayMessage(continueMessage2, coordinateX, coordinateY + 1, 0);

        speaker(1, 30, 25, -1); // amout of BEEPs | BEEP tone | length of BEEP | LED

        while (waitForInput(button_confirm))
        {
            // wait for input
        }
        wasUserGreeted = true;
    }

    clearScreen();

    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);

    intializeTable(factor1); // Resetting values in factor tables
    intializeTable(factor2); // with every reset of the calculator

    enterNumber(factor1);
    enterOperation();
    enterNumber(factor2);

    convertedFactor[0] = convert(factor1);
    convertedFactor[1] = convert(factor2);

    result = solve(convertedFactor, operation);
    if (divideByZero == false && LED_overflow == false)
    {
        speaker(1, 30, 25, greenLED);
        printResult(result);
        while (waitForInput(button_confirm))
        {
            // wait for input
        }
        clearScreen();
        decToBin(result);
        while (waitForInput(button_confirm))
        {
            // wait for input
        }
        clearScreen();
        decToHex(result);
    }
    // RESTART
    while (waitForInput(button_restart))
    {
        // wait for input
    }
    operation = " "; // Resetting operation type
    shiftX = 1;      // Resetting offset back to the beginning of the screen
    divideByZero = false;
    LED_overflow = false;
    wholeFactorFilled = false;
}

void decToHex(long result) // Conversion from decimal to hexadecimal
{
    char hexResult[10];
    intializeTableBinHex(hexResult);
    int counter = 0;
    long tempResult = result;
    while (tempResult > 0)
    {
        int remainder = tempResult % 16;
        if (remainder < 10)
        {
            hexResult[counter] = '0' + remainder; // Digits 0-9
        }
        else
        {
            hexResult[counter] = 'A' + (remainder - 10); // Letters A-F
        }
        counter++;
        tempResult = tempResult / 16;
    }

    counter = 1;
    for (int i = 9; i >= 0; i--)
    {
        if (hexResult[i] != 'X')
        {
            lcd.setCursor(counter, 0);
            lcd.print(hexResult[i]); // printing hex values on the screen
            counter++;
        }
    }
}
void decToBin(long result) // conversion from decimal to binary
{
    char binResult[10];
    intializeTableBinHex(binResult);
    int counter = 0;
    long tempResult = result;
    while (tempResult >= 1)
    {
        if (tempResult % 2 == 1)
        {
            binResult[counter] = '1';
        }
        else
        {
            binResult[counter] = '0';
        }
        counter++;
        tempResult = tempResult / 2;
    }
    counter = 1;
    for (int i = 9; i >= 0; i--)
    {
        if (binResult[i] != 'X')
        {
            lcd.setCursor(counter, 0);
            lcd.print(binResult[i]); // printing binary values on the screen
            counter++;
        }
    }
}

void speaker(int numberOfBeeps, int beepTone, int beepLength, int LED)
{
    if (LED != -1)
    {
        digitalWrite(LED, HIGH);
    }
    for (int i = numberOfBeeps; i > 0; i--)
    {
        tone(buzzer, beepTone);
        delay(beepLength);
        noTone(buzzer);
        delay(beepLength);
    }
}

void printResult(long result)
{
    resultLength = getResultLength(result);

    if (shiftX <= 8 && (shiftX + 3 + resultLength < 15)) // Print values in a single line without erasing factor2 | "+3" is an offset reserved for:( " ", "=", " " ) characters.
    {
        shiftX += 2;
        lcd.setCursor(shiftX, 0);
        lcd.print("=");

        shiftX += 2;
        lcd.setCursor(shiftX, 0);
        lcd.print(result);
    }
    else // The result won't fit, clear the screen and display the result on the upper and lower segments
    {
        clearScreen();
        lcd.setCursor(1, 0);
        lcd.print(result);
    }
}

int getResultLength(long result)
{
    long tempResult = result;
    int length = 0;
    while (tempResult != 0)
    {
        tempResult = tempResult / 10;
        length++;
    }
    return length;
}

long convert(int factor[]) // Convert values in table to decimal based on their position in table
{
    long result_int = 0;
    for (int i = 0; i < 12; i++)
    {
        if (factor[i] != -1)
        {
            result_int = result_int * 10 + factor[i];
        }
    }
    return result_int;
}

long solve(long convertedFactor[], char operation) // Solve factors based on chosen operation
{
    long checkValue = 0;
    switch (operation)
    {
    case '+':
        checkValue = convertedFactor[0] + convertedFactor[1];
        if (checkValue <= 0)
        {
            clearScreen();
            displayMessage(NumberTooBig1, coordinateX + 1, coordinateY, 0);
            displayMessage(NumberTooBig2, coordinateX + 6, coordinateY + 1, 0);
            speaker(2, 29, 300, yellowLED); // amout of BEEPs | BEEP tone | length of BEEP | LED
            LED_overflow = true;
        }
        return (long)convertedFactor[0] + convertedFactor[1];
    case '-':
        if ((convertedFactor[1] > 0 && convertedFactor[0] < LONG_MIN + convertedFactor[1]) ||
            (convertedFactor[1] < 0 && convertedFactor[0] > LONG_MAX + convertedFactor[1]))
        {
            clearScreen();
            displayMessage(NumberTooBig1, coordinateX + 1, coordinateY, 0);
            displayMessage(NumberTooBig2, coordinateX + 6, coordinateY + 1, 0);
            speaker(2, 29, 300, yellowLED); // amout of BEEPs | BEEP tone | length of BEEP | LED
            LED_overflow = true;
        }
        return (long)convertedFactor[0] - convertedFactor[1];
    case '*':
        if (convertedFactor[1] != 0 && (convertedFactor[0] > LONG_MAX / convertedFactor[1] || convertedFactor[0] < LONG_MIN / convertedFactor[1]))
        {
            clearScreen();
            displayMessage(NumberTooBig1, coordinateX + 1, coordinateY, 0);
            displayMessage(NumberTooBig2, coordinateX + 6, coordinateY + 1, 0);
            speaker(2, 29, 300, yellowLED); // amout of BEEPs | BEEP tone | length of BEEP | LED
            LED_overflow = true;
        }
        return (long)convertedFactor[0] * convertedFactor[1];
    case '/':
        if (convertedFactor[1] == 0)
        {
            clearScreen();
            displayMessage(divideByZero1, coordinateX + 1, coordinateY, 0);
            displayMessage(divideByZero2, coordinateX + 4, coordinateY + 1, 0);
            speaker(3, 29, 300, redLED); // amout of BEEPs | BEEP tone | length of BEEP | LED
            divideByZero = true;
        }
        if (!divideByZero)
        {
            checkValue = convertedFactor[0] / convertedFactor[1];
            if (checkValue < 0)
            {
                clearScreen();
                displayMessage(NumberTooBig1, coordinateX + 1, coordinateY, 0);
                displayMessage(NumberTooBig2, coordinateX + 6, coordinateY + 1, 0);
                speaker(2, 29, 300, yellowLED); // amout of BEEPs | BEEP tone | length of BEEP | LED
                LED_overflow = true;
            }
        }
        return (long)convertedFactor[0] / convertedFactor[1];
    }
}

void enterNumber(int factor[]) // Receiving value from the potentiometer to establish factors. Every confirmed value by the user will be stored in memory via int tables
{
    int number = 0; // variable used to display converted number from potentiometer value
    int tempY = 0;
    if (operation == 32) // if the operation is empty - the char hasn't been placed, clear the screen
    {
        clearScreen();
    }

    if (wholeFactorFilled) // if upper segment have been filled - reset shiftX value and start printing values on lower segment
    {
        shiftX = 1;
        tempY = 1;
    }

    for (int i = 0; i < 12; i++)
    {
        while (waitForInput(button_confirm))
        {
            number = convertForNumber(analogRead(potentiometer));
            lcd.setCursor(shiftX, tempY);
            lcd.print(number);
            factor[i] = number;
            if (!(waitForInput(button_place_operation)))
            {
                goto exitLoop;
            }
        }
        delay(300);
        while (!(waitForInput(button_confirm)))
        {
            // wait for button release
        }
        shiftX += 1;
        if (shiftX == 15)
        {
            shiftX = 1;
            tempY = 1;
        }
        if (i == 11)
        {
            wholeFactorFilled = true;
        }
    }
exitLoop:
    delay(0);
}

void enterOperation() // Receiving value from the potentiometer to establish operation type
{
    shiftX += 2;
    while (waitForInput(button_confirm))
    {
        operation = convertForOperation(analogRead(potentiometer));
        lcd.setCursor(shiftX, 0);
        lcd.print(operation);
    }
    delay(100);
    while (!(waitForInput(button_confirm)))
    {
        // wait for button release
    }
    shiftX += 2;
}

void intializeTable(int factor[])
{
    for (int i = 0; i < 12; i++)
    {
        factor[i] = -1;
    }
}

void intializeTableBinHex(char table[])
{
    for (int i = 0; i < 10; i++)
    {
        table[i] = 'X';
    }
}

void displayMessage(String message, int coordinateX, int coordinateY, int delayMs)
{
    lcd.setCursor(coordinateX, coordinateY);
    lcd.print(message);
    delay(delayMs);
}

bool waitForInput(int button)
{
    return digitalRead(button);
}

int convertForNumber(int potentiometer)
{
    switch (potentiometer)
    {
    case 0 ... 102:
        return 0;
    case 103 ... 205:
        return 1;
    case 206 ... 308:
        return 2;
    case 309 ... 411:
        return 3;
    case 412 ... 514:
        return 4;
    case 515 ... 617:
        return 5;
    case 618 ... 720:
        return 6;
    case 721 ... 823:
        return 7;
    case 824 ... 926:
        return 8;
    case 927 ... 1023:
        return 9;
    }
    return -1; // error
}

char convertForOperation(int potentiometer)
{
    switch (potentiometer)
    {
    case 0 ... 255:
        return '+';
    case 256 ... 511:
        return '-';
    case 512 ... 767:
        return '*';
    case 768 ... 1023:
        return '/';
    }
    return 'X'; // error
}

void clearScreen() // Iterate through Screen clearing it's contents
{
    int tempY = 0;
    for (int i = 15; i >= -1; i--)
    {
        if (i == -1)
        {
            i = 15;
            tempY++;
            if (tempY == 2)
            {
                break;
            }
        }
        lcd.setCursor(i, tempY);
        lcd.print(" ");
    }
}
