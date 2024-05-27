#include <Arduino.h>
#include <interface.h>

static char commandBuffer[COMMAND_BUFF_SIZE]; 
static int position = 0;

void setup() {
  InitializeInterface();
}

void loop() {
  if(ReadUserInterface(commandBuffer, COMMAND_BUFF_SIZE, &position)) {
    // Look for the right command if it is available


    FlushStream();
    Serial.print("\nArduinOS> ");
  }
}

