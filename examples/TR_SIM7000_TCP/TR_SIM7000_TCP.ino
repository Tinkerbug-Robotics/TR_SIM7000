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
#include <SoftwareSerial.h>

// Software serial pins to talk to SIM7000
#define PIN_TX     12
#define PIN_RX     13
SoftwareSerial sim7000Serial(PIN_RX,PIN_TX);

// SIM7000 power key and reset key pins
#define PWRKEY 23
#define RESET 6

// APN for mobile provider, for example "wholesale" for Tello, "hologram" for Hologram
char apn[] = "hologram";

// Create instance of sim7000 modem
TR_SIM7000 sim7000(&sim7000Serial, PWRKEY, RESET, apn);

void setup() 
{
    // USB serial
    Serial.begin(115200);

    // SIM7000 serial
    sim7000Serial.begin(19200);

    // Turn on SIM7000
    Serial.print("Turning ON SIM7000 ... ");
    if(sim7000.turnON())
    {
        Serial.println("SIM7000 ON !");
    }

    // Check baud rate
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

    // Check SIM card
    Serial.print("Check SIM card ... ");
    if (sim7000.checkSIMStatus())
    {
        Serial.println("SIM card READY");
    }
    else
    {
        Serial.println("SIM card ERROR");
        while (1);
    }
    delay(500);
    
    Serial.print("Set network: preferred mode LTE only and Narrow Band IOT ... ");
    if (sim7000.setNetMode(sim7000.eNB))
    {
        Serial.println("Mode set");
    }
    else
    {
        Serial.println("Failed to set mode");
    }
    delay(500);

    // Get signal quality
    Serial.print("Get signal quality (0 to 30) ...");
    int signal_strength = sim7000.checkSignalQuality();
    Serial.print("Signal Strength = ");Serial.println(signal_strength);
    delay(500);

    // Open connection to provider
    Serial.println("Open connection with provider's service ... ");
    if (sim7000.attacthService())
    {
        Serial.println("Connection opened");
    }
    else
    {
        Serial.println("Failed to open connection");
        while (1);
    }
    delay(200);
    
//  Serial.println("Connecting......");
//  if (sim7000.openNetwork(sim7000.eTCP, "www.taobao.com", 80)) {  //Start Up TCP or UDP Connection
//    Serial.println("Connect OK");
//  } else {
//    Serial.println("Fail to connect");
//    while (1);
//  }
//  sim7000.send("HEAD/HTTP/1.1\r\nHost:www.taobao.com\r\nConnection:keep-alive\r\n\r\n");    //Send Data Through TCP or UDP Connection
//  dataNum = sim7000.recv(buff, 350);                                       //Receive data
//  Serial.print("dataNum=");
//  Serial.println(dataNum);
//  Serial.println(buff);
//  delay(500);
//  if (sim7000.closeNetwork()) {                                            //End the connection
//    Serial.println("Close connection");
//  } else {
//    Serial.println("Fail to close connection");
//  }
//  delay(2000);
//  sim7000.turnOFF();                                                       //Turn OFF SIM7000
}

void loop() 
{
    delay(1000);
}
