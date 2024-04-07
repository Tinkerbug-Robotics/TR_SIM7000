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

#ifndef _TR_SIM7000_H_
#define _TR_SIM7000_H_

#include "Arduino.h"
//#include "String.h"

#define ON  0
#define OFF 1

class TR_SIM7000
{
    public:
    
    /**
     * @fn TR_SIM7000
     * @brief SIM7000 constructor
     * @return None
     */
    TR_SIM7000();
    
   /**
    * @fn TR_SIM7000
    * @brief SIM7000 destructor
    */
    ~TR_SIM7000(){};
    
    /**
      * @enum eNet
      * @brief Select data communication mode
      */
      typedef enum
      {
          eGPRS,
          eNB,
      }eNet;
      
   /**
     * @fn init
     * @brief Initialize the library
     * @param pwr_pin Pin number for GPIO controlling SIM7000 power key
     * @param reset_pin Pin number for GPIO controlling SIM7000 reset
     * @param apn_in Provider's APN
     * @param host_in NTRIP caster host
     * @param tcp_port_in NTRIP caster port to use
     * @param mntpnt_in NTRIP caster mount point to connect to
     * @param user_in NTRIP caster user id
     * @param psw_in NTRIP caster password
     * @param info_in NTRIP caster info
     * @param input_port SIM7000 serial port
     * @return None
     */
    void init(int pwr_pin, 
              int reset_pin,
              char* apn_in,
              char* host_in,
              int &tcp_port_in,
              char* mntpnt_in,
              char* user_in,
              char* psw_in,
              char* info_in,
              Stream &input_port);
               
   /**
     * @fn connect
     * @brief Connect SIM7000 to network
     */
   bool connect();
  
   /**
    * @fn setNetMode
    * @brief Set net mode
    * @param net The net mode
    * @n    GPRS: GPRS mode
    * @n    NB:   NB-IOT mode
    * @return bool type, indicating the status of setting
    * @retval ture Success 
    * @retval false Failed
    */
   bool setNetMode(eNet net);

   /**
    * @fn checkSignalQuality
    * @brief Check signal quality
    * @return 0-30:Signal quality
    */
   int checkSignalQuality(void);
  
   /**
    * @fn attacthService
    * @brief Open the connection
    * @return bool type, indicating the status of opening the connection
    * @retval true Success 
    * @retval false Failed
    */
   bool attachService(void);

   /**
    * @fn turnON
    * @brief Turn ON SIM7000
    * @return bool type, indicating the status of turning on
    * @retval true Success 
    * @retval false Failed
    */
   bool turnON(void);
  
   /**
    * @fn setBaudRate
    * @brief Set baud rate to avoid garbled
    * @param rate Baud rate value
    * @n    Possible values:1200 2400 4800 9600 19200 38400
    * @note 19200 or lower works better with software serial
    * @return bool type, indicating the status of setting
    * @retval true Success 
    * @retval false Failed
    */
   bool setBaudRate(long rate);

   /**
    * @fn checkSIMStatus
    * @brief Check SIM card
    * @return bool type, indicating the status of checking SIM card
    * @retval true Success 
    * @retval false Failed
    */
   bool checkSIMStatus(void);

   /**
    * @fn closeNetwork
    * @brief End the connection
    * @return bool type, indicating the status of closing Network
    * @retval true Success 
    * @retval false Failed
    */
   bool closeNetwork(void);
  
   /**
    * @fn establishTCPConnectionClient
    * @brief Establish a TCP connection in client mode
    * @return bool type, indicating the status of establishing TCP connection
    * @retval true Success 
    * @retval false Failed
    */
   bool establishTCPConnectionClient();
   
   /**
    * @fn establishTCPConnectionServer
    * @brief Establish a TCP connection in server mode
    * @return bool type, indicating the status of establishing TCP connection
    * @retval true Success 
    * @retval false Failed
    */
   bool establishTCPConnectionServer();
  
   /** 
    * @fn readTCP
    * @brief Read data from TCP and populate buffer
    * @param buff Buffer to populate with TCP response
    * @param maxlen Maximum length of data to populate
    */
   uint16_t readTCP(char *buff,
                    uint16_t maxlen);
                    
    boolean checkTCP(void);

  /**
   * @fn send
   * @brief Send data with specify the length
   * @param buf The buffer for data to be send
   * @param len The length of data to be send
   * @return bool type, indicating status of sending
   * @retval true Success 
   * @retval false Failed
   */
  bool send(char *buf,size_t len);

  /**
   * @fn send
   * @brief Send data
   * @param data The data to send
   * @return bool type, indicating status of sending
   * @retval true Success 
   * @retval false Failed
   */
  bool send(char *data);

  
private:

    // Baud rate for communicating with SIM7000
	long baud_rate = 19200;
    
    // Serial port (passed in on init) for communicating with SIM7000
	Stream *sim7000Serial;
    
    // Power key (passed in on init) for enabling the SIM7000
    uint8_t PWRKEY = 23;
    
    // Reset key (passed in on init) for resetting SIM7000
    uint8_t RESET = 6;
    
    // Provider network APN (passed in on init)
    char* APN;
    
    // NTRIP caster host (IP address)
    char* host;
    
    // NTRIP caster port to use
    int tcp_port;
    
    // NTRIP caster mount point to connect to
    char* mntpnt;
    
    // NTRIP caster user id
    char* user;
    
    // NTRIP caster password
    char* psw;
    
    // NTRIP caster info
    char* info;
    
    /**
     * @fn checkSendCmd
     * @brief Send a command to SIM7000 and check response
     * @param cmd Command to send
     * @param resp Desired response from SIM7000
     * @param timeout Amount of time (milliseconds) to wait for response
     * @return bool type, indicates if desired response was received
     * @retval true Desired response received 
     * @retval false Desired response not received
     */
    bool checkSendCmd(const char* cmd, 
                      const char* resp, 
                      uint32_t timeout = 1000);
                      
    /**
     * @fn sendCmd
     * @brief Send a command to SIM7000 without checking for response
     * @param cmd Command to send
     */
    void sendCmd(const char* cmd);
    
    /**
     * @fn sendCmd
     * @brief Send a command to SIM7000 without checking for response
     * @param cmd Command to send
     */
     void sendCmd(String cmd);
            
    /**
     * @fn checkReadable
     * @brief Checks how many characters are in the SIM7000 serial queue
     * @return Number of characters in SIM7000 serial queue
     */
    int checkReadable(void);
                      
    /**
     * @fn cleanBuffer
     * @brief Clears data from the provided buffer
     * @param buffer Buffer to clear
     * @param length Length of buffer to clear
     */
    void cleanBuffer(char *buffer,
                     int length);
    
    /**
     * @fn readBuffer
     * @brief Reads data from SIM7000 serial into passed in buffer
     * @param buffer Buffer to read to
     * @param max_length Maximum length to read
     * @param timeout Maximum amount of time to read for
     */
    uint16_t readBuffer(char *buffer, 
                        uint16_t max_length, 
                        uint32_t timeout = 1000);
                        
    int signalRSSI(int signal_quality);
    
    String getSignalQualityDescriptor(int signal_quality);
    
};

#endif
