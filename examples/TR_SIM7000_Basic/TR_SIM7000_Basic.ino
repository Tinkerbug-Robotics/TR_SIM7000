/**********************************************************************
*
* MIT License
*
* Copyright (c) 2024 Tinkerbug Robotics
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* 
* Authors: 
* Christian Pedersen; tinkerbug@tinkerbugrobotics.com
* 
**********************************************************************/

#include <TR_SIM7000.h>

// Software serial pins to talk to SIM7000
#define PIN_TX     12
#define PIN_RX     13
SoftwareSerial mySerial(PIN_RX,PIN_TX);

// SIM7000 power key and reset key pins
#define PWRKEY 23
#define RESET 6

// Create instance of sim7000 modem
TR_SIM7000 sim7000(&mySerial, PWRKEY, RESET);

void setup() 
{
    // USB serial
    Serial.begin(115200);

    // SIM7000 serial
    mySerial.begin(19200);
    
    Serial.print("Turning ON SIM7000 ... ");
    if(sim7000.turnON())
    {
        Serial.println("SIM7000 ON !");
    }

    Serial.print("Set baud rate ... ");
    while(1)
    {
    if(sim7000.setBaudRate(19200))
    {
        Serial.print("Succesfully baud rate to 19200");
            break;
        }
        else
        {
            Serial.print(".");
            delay(1000);
        }
    }
    Serial.println("");
    
  Serial.println("For example, if you type AT\\r\\n, OK\\r\\n will be responsed!");
  Serial.println("Enter your AT command :");
}

void loop() {
  mySerial.listen();
  while(mySerial.available()){
    Serial.write(mySerial.read());
  }
  mySerial.flush();
  while(Serial.available()){
    mySerial.write(Serial.read());
  }
}
