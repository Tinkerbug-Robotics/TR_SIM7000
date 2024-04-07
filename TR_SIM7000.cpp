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

#include<base64.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

TR_SIM7000::TR_SIM7000()
{}

void TR_SIM7000::init(int pwr_pin, 
                      int reset_pin,
                      char* apn_in,
                      char* host_in,
                      int &tcp_port_in,
                      char* mntpnt_in,
                      char* user_in,
                      char* psw_in,
                      char* info_in,
                      Stream &input_port)
{
    sim7000Serial = &input_port;
    PWRKEY = pwr_pin;
    RESET = reset_pin;
    APN = apn_in;
    host = host_in;
    tcp_port = tcp_port_in;
    mntpnt = mntpnt_in;
    user = user_in;
    psw = psw_in;
    info = info_in;
}

bool TR_SIM7000::connect()
{
    // Turn on SIM7000
    Serial.print("Turning On SIM7000 ... ");
    if(turnON())
    {
        Serial.println("SIM7000 is On");
    }

    // Check SIM card
    Serial.print("Checking SIM card ... ");
    if (checkSIMStatus())
    {
        Serial.println("SIM card is ready");
    }
    else
    {
        Serial.println("SIM card ERROR");
        while (1);
    }
    
    Serial.print("Setting network: preferred mode LTE only and CAT-M ... ");
    if (setNetMode(eNB))
    {
        Serial.println("Mode set");
    }
    else
    {
        Serial.println("Failed to set mode");
    }
    delay(100);

    Serial.print("Closing any existing network connection ... ");
    if (closeNetwork())
    {
        Serial.println("Network connections closed");
    }
    else
    {
        Serial.println("Failed to close network connections");
    }
    delay(1000);

    // Get signal quality
    Serial.print("Getting signal quality ...");
    int signal_strength = checkSignalQuality();
    if (signal_strength < 2)
    {
        while (signal_strength < 2)
        {
            signal_strength = checkSignalQuality();
            Serial.print("Signal strength of ");
            Serial.print(signal_strength);
            Serial.print(" or ");
            Serial.print(signalRSSI(signal_strength));
            Serial.print(" dBm RSSI is ");
            Serial.println(getSignalQualityDescriptor(signal_strength));
            delay(2000);

        }
    }
    else
    {
            Serial.print("Signal strength of ");
            Serial.print(signal_strength);
            Serial.print(" or ");
            Serial.print(signalRSSI(signal_strength));
            Serial.print(" dBm RSSI is ");
            Serial.println(getSignalQualityDescriptor(signal_strength));
    }
    
    // Open connection to provider
    Serial.println("Opening connection with provider's service ... ");
    if (attachService())
    {
        delay(100);
        Serial.println("Connection opened");
    }
    else
    {
        Serial.println("Failed to open connection");
        while (1);
    }
    delay(200);
    
    return true;
}

bool TR_SIM7000::turnON(void)
{
    pinMode(RESET,OUTPUT);
    delay(100);
    // Setting the RESET pin to high pulls the SIM7000's reset to low using a
    // transistor
    digitalWrite(RESET, HIGH);
    delay(500);
    // Set the reset back to high by releasing the voltage on the transistor
    digitalWrite(RESET, LOW);

    // Cycle the power key to startup the SIM7000G
    pinMode(PWRKEY,OUTPUT);
    digitalWrite(PWRKEY, LOW);
    delay(1100);
    digitalWrite(PWRKEY, HIGH);
    delay(7000);
    
    while(!checkSendCmd("AT\r\n", "OK", 100))
    {
        Serial.print(".");
        delay(1000);
    }
    
    return true;

}

bool TR_SIM7000::setBaudRate(long rate)
{
    uint8_t count = 0;
    
    if (!(rate == 1200 || rate == 2400 || rate == 4800 || rate == 9600 ||
          rate == 19200 || rate == 38400))
    {
        Serial.print(rate);Serial.println("is an invalid rate for the SIM7000");
        return false;
    }
       
    char baud_command[20];
    sprintf(baud_command, "AT+IPR=%d\r\n", rate);
    
    // Try up to 3 times to set the baud rate
    while(count <3)
    {
        if(checkSendCmd(baud_command,"OK"))
        {
            baud_rate = rate;
            break;
        }
        else
        {
            count++;
            delay(200);
        }
    }

    // Return false if tries were exhausted, else return true
    if(count == 3)
        return false;
    else
        return true;
}

bool TR_SIM7000::checkSIMStatus(void)
{
    uint8_t count = 0;
    while(count < 3)
    {
        if(checkSendCmd("AT\r\n","OK"))
        {
            break;
        }
        else
        {
            count++;
            delay(300);
        }
    }
    if(count == 3)
    {
        return false;
    }
    
    count = 0;
    while(count < 3)
    {
        if(checkSendCmd("AT+CPIN?\r\n","READY"))
        {
            break;
        }
        else
        {
            count++;
            delay(300);
        }
    }
    
    if(count == 3)
        return false;
    
    return true;
}

bool TR_SIM7000::setNetMode(eNet net)
{
    if(net == eNB)
    {
  	    if(checkSendCmd("AT+CNMP=38\r\n","OK"))
        {
            delay(300);
            if(checkSendCmd("AT+CMNB=1\r\n","OK"))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else if(net == eGPRS)
    {
        if(checkSendCmd("AT+CNMP=13\r\n","OK"))
        {
            delay(300);
            if(checkSendCmd("AT+CMNB=1\r\n","OK"))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    else
    {
        Serial.println("No such mode!");
    }
    return false;
}

bool TR_SIM7000::attachService(void)
{
    // Attach to GPRS service
    char gprsBuffer[32];
    cleanBuffer(gprsBuffer,32);
    sendCmd("AT+CGATT=1\r\n");
    while(1)
    {
        readBuffer(gprsBuffer, 320);
        if(NULL != strstr(gprsBuffer, "OK"))
        {
            delay(100);
            break;  
        }
        if(NULL != strstr(gprsBuffer, "ERROR"))
        {
            return false;
        }
    }
    
    // Set providers APN
    char apn_command[64];
    sprintf(apn_command, "AT+CSTT=\"%s\"\r\n", APN);
    sendCmd(apn_command);
    cleanBuffer(gprsBuffer,32);
    while(1)
    {
        readBuffer(gprsBuffer, 32);
        if(NULL != strstr(gprsBuffer, "OK"))
        {
            Serial.print("Provider APN set to ");
            Serial.println(APN);
            delay(200);
            break;
        }
        else if(NULL != strstr(gprsBuffer,"ERROR"))
        {
            Serial.println("Error setting provider APN");
            return false;
        }
    }
  
    // Open wireless connection with GPRS
    sendCmd("AT+CIICR\r\n");
    while(1)
    {
        readBuffer(gprsBuffer, 32, 4000);
        if(NULL != strstr(gprsBuffer, "OK"))
        {
            Serial.println("Wireless connection opened");
            delay(200);
            break;
        }
        else if(NULL != strstr(gprsBuffer,"ERROR"))
        {
            Serial.println("Error opening wireless connection");
            return false;
        }
    }
    
    // Open wireless connection with GPRS
    cleanBuffer(gprsBuffer,32);
    sendCmd("AT+CIFSR\r\n");
    while(1)
    {
        int buff_length = readBuffer(gprsBuffer, 32, 4000);
        if(NULL != strstr(gprsBuffer,"ERROR"))
        {
            Serial.println("Error reading IP address");
            return false;
        }
        else
        {
            uint8_t first_char = 11;
            char* ip_addr = (char*)malloc((strlen(gprsBuffer) - 10) * sizeof(char));
            for (int i=first_char;i<buff_length-2;i++)
            {
                ip_addr[i-first_char] = gprsBuffer[i];
            }
            Serial.print("IP address is: ");Serial.println(ip_addr);
            delay(200);
            break;
        }
    }
    
    sendCmd("AT+CEREG?\r\n");
    while(1)
    {
        readBuffer(gprsBuffer, 32, 4000);
        if(NULL != strstr(gprsBuffer, "+CEREG: 0,0"))
        {
            Serial.println("ERROR: Not registered to network (+CEREG: 0,0)");
            delay(2000);
            return false;
        }
        else if(NULL != strstr(gprsBuffer, "+CEREG: 0,1"))
        {
            Serial.println("Registered to home network");
            break;
        }
        else if(NULL != strstr(gprsBuffer, "+CEREG: 0,2"))
        {
            Serial.println("Searching for network (+CEREG: 0,2)");
            delay(2000);
            break;
        }
        else if(NULL != strstr(gprsBuffer, "+CEREG: 0,3"))
        {
            Serial.println("ERROR: Network registration denied (+CEREG: 0,3)");
            delay(2000);
            return false;
        }
        else if(NULL != strstr(gprsBuffer, "+CEREG: 0,4"))
        {
            Serial.println("ERROR: Undefined response (+CEREG: 0,4)");
            delay(2000);
            return false;
        }
        else if(NULL != strstr(gprsBuffer, "+CEREG: 0,5"))
        {
            Serial.println("Registered as roaming");
            break;
        }
        else if(NULL != strstr(gprsBuffer,"ERROR"))
        {
            Serial.println("CEREG Error");
            return false;
        }
        delay(1000);
    }
    
    sendCmd("AT+CGATT?\r\n");
    while(1)
    {
        readBuffer(gprsBuffer, 32, 4000);
        if(NULL != strstr(gprsBuffer, "OK"))
        {
            Serial.println("Attached to GPRS service");
            delay(200);
            break;
        }
        else if(NULL != strstr(gprsBuffer,"ERROR"))
        {
            Serial.println("Failure to attach to GPRS service");
            return false;
        }
    }
    

    return true;
}

int TR_SIM7000::checkSignalQuality(void)
{
    char signalBuffer[26];
    int i = 0,j = 0,k = 0;
    char *signalQuality;
    cleanBuffer(signalBuffer,26);
    sendCmd("AT+CSQ\r\n");
    readBuffer(signalBuffer,26);
    //Serial.println(signalBuffer);
    if(NULL != (signalQuality = strstr(signalBuffer, "+CSQ:")))
    {
        i = *(signalQuality + 6) - 48;
        j = *(signalQuality + 7) - 48;
        k =  (i * 10) + j;
    }
    else
    {
        return 0;
    }
    
    if( k == 99)
    {
        return 0;
    }
    else
    {
        return k;
    }
}

int TR_SIM7000::signalRSSI(int signal_quality)
{
    int rssi = 0;
    
    if(signal_quality == 0)
        rssi = -115;
    else if (signal_quality == 1)
        rssi = -111;
    else
    {
        rssi = (-110+(2*(signal_quality-2)));
    }
    
    return rssi;
}

String TR_SIM7000::getSignalQualityDescriptor(int signal_quality)
{
    String signal_quality_indicator = "";
    if(signal_quality >= 18)
        signal_quality_indicator = "Great";
    else if(signal_quality >= 13)
        signal_quality_indicator = "Good";
    else if(signal_quality >= 8)
        signal_quality_indicator = "Average";
    else if(signal_quality >= 3)
        signal_quality_indicator = "Below Average";
    else if(signal_quality < 3)
        signal_quality_indicator = "Poor";
    
    return signal_quality_indicator;
}

bool TR_SIM7000::establishTCPConnectionClient()
{ 
    // Create new connection
    char num[4];
    char resp[1024];
    sendCmd("AT+CIPSTART=\"TCP\",\"");
    sendCmd(host);
    sendCmd("\",");
    itoa(tcp_port, num, 10);
    sendCmd(num);
    sendCmd("\r\n");
    
    Serial.print("Establishing TCP connection ...");
    
    cleanBuffer(resp, 1024);
    bool connection_success = false;
    while(1)
    {
        while(checkReadable())
        {
            readBuffer(resp, 1024);
            if(NULL != strstr(resp,"CONNECT OK"))
            {
                Serial.print("Connection succesful, ");
                connection_success = true;
            }
            if(NULL != strstr(resp,"ERROR"))
            {
                Serial.println("Connection rejected");
                return false;
            }
        }
        if(connection_success)
            break;
    }
    
    char gprsBuffer[32];
    sendCmd("AT+CIPSEND\r\n");
    cleanBuffer(gprsBuffer,32);
    while(1)
    {
        readBuffer(gprsBuffer, 32);
        if(NULL != strstr(gprsBuffer, ">"))
        {
            Serial.println("Ready to send");
            delay(100);
            break;
        }
        else if(NULL != strstr(gprsBuffer,"ERROR"))
        {
            Serial.println("CIPSEND failed");
            return false;
        }
    }
    
    String p="GET /";
    String auth="";
    Serial.print("Requesting NTRIP ... ");
    
    p = p + mntpnt + String(" HTTP/1.0\r\n"
        "User-Agent: NTRIPClient for Arduino v1.0\r\n"
    );
    
    if (strlen(user)==0) 
    {
        p = p + String(
            "Accept: */*\r\n"
            "Connection: close\r\n"
            );
    }
    else 
    {
        auth = base64::encode(String(user) + String(":") + psw);

        p = p + String("Authorization: Basic ");
        p = p + auth;
        p = p + String("\r\n");
    }
    p = p + String("\r\n");
    
    //Serial.println(p);
    sendCmd(p);
    // Indicate end of write
    sim7000Serial->write(0x1a);
    
    char snip_resp[20000];
    cleanBuffer(snip_resp, 20000);

    while(1)
    {
        while(checkReadable())
        {
            readBuffer(snip_resp, 20000,2000);
            //Serial.print("snip_resp = ");Serial.println(snip_resp);
            if(NULL != strstr(snip_resp,"ICY 200 OK"))
            {
                Serial.println("Received expected response from caster");
                return true;
            }
            if(NULL != strstr(snip_resp,"ERROR"))
            {
                Serial.println("Connection rejected");
                return false;
            }
        }
    }
    return true;
}

bool TR_SIM7000::establishTCPConnectionServer()
{ 
    // Create new connection
    char num[4];
    char resp[1024];
    sendCmd("AT+CIPSTART=\"TCP\",\"");
    sendCmd(host);
    sendCmd("\",");
    itoa(tcp_port, num, 10);
    sendCmd(num);
    sendCmd("\r\n");
    
    Serial.print("Establishing TCP connection ...");
    cleanBuffer(resp, 1024);
    bool connection_success = false;
    while(1)
    {
        while(checkReadable())
        {
            readBuffer(resp, 1024);
            if(NULL != strstr(resp,"CONNECT OK"))
            {
                Serial.print("Connection succesful, ");
                connection_success = true;
            }
            if(NULL != strstr(resp,"ERROR"))
            {
                Serial.println("Connection rejected");
                return false;
            }
        }
        if(connection_success)
            break;
    }
    
    // Build the request string
    char get1[] = "SOURCE ";
    char get2[] = "\r\n";
    char get3[] = "Source-Agent: AT_NTRIP v1.0\r\n";
    char get4[] = "STR: ";
    char get5[] = "\x1A";

    char* data_to_send;
    data_to_send = (char*)malloc(strlen(get1)+strlen(psw)+1+strlen(mntpnt)
                                +strlen(get2)+strlen(get3)+strlen(get4)
                                +strlen(get2)+strlen(get2));

    strcpy(data_to_send, get1);
    strcat(data_to_send, psw);
    strcat(data_to_send, " ");
    strcat(data_to_send, mntpnt);
    strcat(data_to_send, get2);
    strcat(data_to_send, get3);
    strcat(data_to_send, get4);
    strcat(data_to_send, get2);
    strcat(data_to_send, info);
    strcat(data_to_send, get2);
    strcat(data_to_send, get5);
    
    // Send the request string
    sendCmd(data_to_send);
    Serial.print("Sent ");
    delay(25);
    Serial.println(data_to_send);

    sendCmd(data_to_send);
    // Indicate end of write
    sim7000Serial->write(0x1a);
    
    Serial.println("TCP Connection Succesful\n");

    return true;
}

uint16_t TR_SIM7000::readTCP(char *buff, uint16_t maxlen)
{
    uint16_t avail;

    // Read buffer to get response
    char gprsBuffer[maxlen];
    cleanBuffer(gprsBuffer,maxlen);
    int i=readBuffer(gprsBuffer,maxlen,500);
    Serial.print("Read TCP data of length ");Serial.println(i);
    
    // Copy buffer to pointer passed in
    memcpy(buff,gprsBuffer,i);
    
    // Return length of data read
    return i;
}

boolean TR_SIM7000::checkTCP(void)
{
    if(checkSendCmd("AT+CIPSTATUS\r\n","STATE: CONNECT OK",1000))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool TR_SIM7000::send(char *data)
{
    char num[4];
    char resp[20];
    int len = strlen(data);
    itoa(len, num, 10);
    sendCmd("AT+CIPSEND=");
    sendCmd(num);
    if(checkSendCmd("\r\n",">"))
    {
        sendCmd(data);
        while(1)
        {
            while(checkReadable())
            {
                readBuffer(resp,20);
                if(NULL != strstr(resp,"OK"))
                {
                    return true;
                }
                if(NULL != strstr(resp,"ERROR"))
                {
                    return false;
                }
            }
        }
    }
    else
    {
    return false;
    }
}

bool TR_SIM7000::send(char *buf, size_t len)
{
    char num[4];
    itoa(len, num, 10);
    sendCmd("AT+CIPSEND=");
    sendCmd(num);
    if(checkSendCmd("\r\n",">"))
    {
        if(checkSendCmd(buf,"OK"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool TR_SIM7000::closeNetwork(void)
{
    if(checkSendCmd("AT+CIPSHUT\r\n","OK",2000))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool TR_SIM7000::checkSendCmd(const char* cmd, 
                              const char* resp, 
                              uint32_t timeout)
{
    char SIMbuffer[100];
    cleanBuffer(&SIMbuffer[0],100);
    sendCmd(cmd);
    readBuffer(&SIMbuffer[0],100, timeout);
    //Serial.println(SIMbuffer);
    if(NULL != strstr(SIMbuffer,resp))
    {
        return  true;
    }
    else
    {
        return  false;
    }
}

void TR_SIM7000::sendCmd(const char* cmd)
{
  sim7000Serial->write(cmd);
}

void TR_SIM7000::sendCmd(String cmd)
{
  sim7000Serial->println(cmd);
}

int TR_SIM7000::checkReadable(void)
{
  return sim7000Serial->available();
}

void TR_SIM7000::cleanBuffer(char *buffer,
                             int length)
{
    int i;
    for(i=0; i < length; i++)
    {
        buffer[i] = '\0';
    }
}

uint16_t  TR_SIM7000::readBuffer(char *buffer, 
                                 uint16_t max_length, 
                                 uint32_t timeout)
{
    uint16_t i = 0;
    uint64_t timecnt = millis();
    while(1)
    {
        if(sim7000Serial->available())
        {
            buffer[i++] =(char)sim7000Serial->read();
            timecnt = millis();
        }
        if(i == max_length)
            return i;
        
        if((millis()-timecnt) > (timeout))
        {
            //Serial.print("buffer = ");Serial.println(buffer);
            //Serial.print("i = ");Serial.println(i);
            break;
        }
    }
    return i;
}

