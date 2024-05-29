#include <interface.h>
#include <Arduino.h>
#include <HardwareSerial.h>

void InitializeInterface()
{
    Serial.begin(19200);
    Serial.println("ArduinOS 1.0 ready.");
    Serial.print("ArduinOS> ");
}

bool ReadUserInterface(char *commandBuffer, int bufferLength, int *position)
{
    char incomingCharacter = 0;

    while (Serial.available())
    {
        incomingCharacter = Serial.read();

        if (incomingCharacter == '\b')
        {
            if(*position > 0) {
                (*position)--;
                Serial.print('\b');
                Serial.print(' ');
                Serial.print('\b');
            }
            continue;
        }

        if (incomingCharacter == '\n' || incomingCharacter == '\t' || incomingCharacter == ' ' || (*position) >= bufferLength - 1)
        {
            commandBuffer[(*position)] = '\0';
            *position = 0;
            return true;
        }

        commandBuffer[(*position)++] = incomingCharacter;
        Serial.print(incomingCharacter);
    }

    return false;
}

void FlushStream()
{
    delay(10);
    while (Serial.available() > 0)
    {
        Serial.read();
        delayMicroseconds(500);
    }
}
