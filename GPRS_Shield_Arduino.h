/*
 * GPRS_Shield_Arduino.h 
 * A library for SeeedStudio seeeduino GPRS shield 
 *  
 * Copyright (c) 2014 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : lawliet zou
 * Create Time: April 2014
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __GPRS_SHIELD_ARDUINO_H__
#define __GPRS_SHIELD_ARDUINO_H__

#include "sim900_Suli.h"

/** GPRS class.
 *  used to realize GPRS communication
 */ 
#define MESSAGE_LENGTH  20
 
enum Protocol {
    CLOSED = 0,
    TCP    = 1,
    UDP    = 2,
};
 
class GPRS
{
public:
    /** Create GPRS instance
     *  @param number default phone number during mobile communication
     */
    GPRS(int tx, int rx, uint32_t baudRate = 9600, const char* apn = NULL, const char* userName = NULL, const char *passWord = NULL);
    
    /** get instance of GPRS class
     */
    static GPRS* getInstance() {
        return inst;
    };
    
    /** initialize GPRS module including SIM card check & signal strength
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int init(void);

    
    /** send text SMS
     *  @param  *number phone number which SMS will be send to
     *  @param  *data   message that will be send to
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int sendSMS(char* number, char* data);

    /** read SMS if getting a SMS message
     *  @param  buffer  buffer that get from GPRS module(when getting a SMS, GPRS module will return a buffer array)
     *  @param  message buffer used to get SMS message
     *  @param  check   whether to check phone number(we may only want to read SMS from specified phone number)
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int readSMS(int messageIndex, char *message, int length);

    /** delete SMS message on SIM card
     *  @param  index   the index number which SMS message will be delete
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int deleteSMS(int index);

    /** call someone
     *  @param  number  the phone number which you want to call
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int callUp(char* number);

    /** auto answer if coming a call
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int answer(void);

//////////////////////////////////////////////////////
/// GPRS
//////////////////////////////////////////////////////  
   /**  Connect the GPRS module to the network.
     *  @return true if connected, false otherwise
     */
    bool join(void);

    /** Disconnect the GPRS module from the network
     *  @returns true if successful
     */
    bool disconnect(void);
    
    /** Close a tcp connection
     *  @returns true if successful
     */
    bool close(void);

    /** Open a tcp/udp connection with the specified host on the specified port
     *  @param socket an endpoint of an inter-process communication flow of GPRS module,for SIM900 module, it is in [0,6]
     *  @param ptl protocol for socket, TCP/UDP can be choosen
     *  @param host host (can be either an ip address or a name. If a name is provided, a dns request will be established)
     *  @param port port
     *  @param timeout wait seconds till connected
     *  @returns true if successful
     */
    bool connect(Protocol ptl, const char * host, int port, int timeout = DEFAULT_TIMEOUT);


    /** Reset the GPRS module
     */
    void reset();

    /** check if GPRS module is readable or not
     *  @returns true if readable
     */
    int readable(void);

    /** wait a few time to check if GPRS module is readable or not
     *  @param socket socket
     *  @param wait_time time of waiting
     */
    int wait_readable(int wait_time);

    /** wait a few time to check if GPRS module is writeable or not
     *  @param socket socket
     *  @param wait_time time of waiting
     */
    int wait_writeable(int req_size);

    /** Check if a tcp link is active
     *  @returns true if successful
     */
    bool is_connected(void);

    /** send data to socket
     *  @param socket socket
     *  @param str string to be sent
     *  @param len string length
     *  @returns return bytes that actually been send
     */
    int send(const char * str, int len);

    /** read data from socket
     *  @param socket socket
     *  @param buf buffer that will store the data read from socket
     *  @param len string length need to read from socket
     *  @returns bytes that actually read
     */
    int recv(char* buf, int len);

    /** convert the host to ip
     *  @param host host ip string, ex. 10.11.12.13
     *  @param ip long int ip address, ex. 0x11223344
     *  @returns true if successful
     */
    bool gethostbyname(const char* host, uint32_t* ip); 
    
    char* getIPAddress();
    
private:
    int checkSIMStatus(void);
    uint32_t str_to_ip(const char* str);
    SoftwareSerial gprsSerial;
    static GPRS* inst;
    PIN_T _powerPin;
    const char* _apn;
    const char* _userName;
    const char* _passWord;
    uint32_t _ip;
    char ip_string[20];
};
#endif
