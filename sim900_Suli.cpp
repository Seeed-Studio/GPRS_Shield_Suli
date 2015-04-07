/*
 * sim900_Suli.cpp
 * A library for SeeedStudio seeeduino GPRS shield 
 *  
 * Copyright (c) 2014 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : lawliet zou
 * Create Time: April 2015
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

#include "sim900_Suli.h"

SoftwareSerial *serialSIM900 = NULL;

void  sim900_init(void * uart_device, int16 uart_num, uint32 baud)
{
    serialSIM900 = (SoftwareSerial*)uart_device;
    suli_uart_init(uart_device, uart_num, baud);
}

char sim900_read_byte(void)
{
    return suli_uart_read_byte(serialSIM900,-1);
}

int sim900_check_readable()
{
    return suli_uart_readable(serialSIM900,-1);
}

int sim900_wait_readable (int wait_time)
{
    unsigned long timerStart; //,timerEnd;
    int dataLen = 0;
    timerStart = suli_millis();
    //timerEnd = 1000*wait_time + timerStart;
    while((unsigned long) (suli_millis() - timerStart) > wait_time * 1000UL) {
        suli_delay_ms(500);
        dataLen = suli_uart_readable(serialSIM900,-1);
        if(dataLen > 0){
            break;
        }
    }
    return dataLen;
}

void sim900_flush_serial()
{
    while(suli_uart_readable(serialSIM900,-1)){
        char c = suli_uart_read_byte(serialSIM900,-1);
    }
}

int sim900_read_buffer(char *buffer,int count, unsigned int timeout, unsigned int chartimeout)
{
    int i = 0;
    unsigned long timerStart, prevChar; //,timerEnd;
    timerStart = suli_millis();
    prevChar = 0;
    //timerEnd = 1000*timeOut + timerStart;
    while(1) {
#if false
    //23/03/2015 WHY CAN YOU USE THIS IF IN sim900_wait_for_resp ARE YOU USING IT ?
        while (suli_uart_readable(serialSIM900,-1)) {
            char c = suli_uart_read_byte(serialSIM900,-1);
            prevChar = suli_millis();
            buffer[i++] = c;
            if(i >= count)break;
        }
        if(i >= count)break;
        //if(millis() > timerEnd) {
        //    break;
        //}
        if ((unsigned long) (suli_millis() - timerStart) > timeout * 1000UL) {
            break;
        }
        //If interchar Timeout => return FALSE. So we can return sooner from this function. Not DO it if we dont recieve at least one char (prevChar <> 0)
        if (((unsigned long) (suli_millis() - prevChar) > chartimeout) && (prevChar != 0)) {
            break;
        }
#else
/** Anyway, Suli is not quickly enough to fetch data from buffer, so 
    we have to fetch our data like this.*/
    //23/03/2015 WHY THIS IF IN sim900_wait_for_resp ARE YOU USING IT ?
        while (serialSIM900->available()) {
            char c = serialSIM900->read();
            prevChar = suli_millis();
            buffer[i++] = c;
            if(i >= count)break;
        }
        if(i >= count)break;
        //if(millis() > timerEnd) {
        //    break;
        //}
        if ((unsigned long) (suli_millis() - timerStart) > timeout * 1000UL) {
            break;
        }
        //If interchar Timeout => return FALSE. So we can return sooner from this function.
        if (((unsigned long) (suli_millis() - prevChar) > chartimeout) && (prevChar != 0)) {
            break;
        }
#endif
    }
    return 0;   
}



void sim900_clean_buffer(char *buffer, int count)
{
    for(int i=0; i < count; i++) {
        buffer[i] = '\0';
    }
}

void sim900_send_cmd(const char* cmd)
{
  suli_uart_send(serialSIM900, -1, (uint8_t*)cmd, strlen(cmd));
}

void sim900_send_cmd(const __FlashStringHelper* cmd)
{
  int i = 0;
  const char *ptr = (const char *) cmd;
  while (pgm_read_byte(ptr + i) != 0x00) {
    suli_uart_send_byte(serialSIM900, -1, pgm_read_byte(ptr + i++));  
  }
}

void sim900_send_cmd_P(const char* cmd)
{
  while (pgm_read_byte(cmd) != 0x00)
    suli_uart_send_byte(serialSIM900, -1, pgm_read_byte(cmd++));  
}

void  sim900_send_char(const char c)
{
  suli_uart_send_byte(serialSIM900, -1, c);
}

void sim900_send_AT(void)
{
    sim900_check_with_cmd("AT\r\n","OK",CMD);
}

/* NOT USED
bool sim900_response_cmp(uint8_t* resp, unsigned int len, unsigned int timeout)
{
    int sum=0;
    unsigned long timerStart,timerEnd;
    timerStart = suli_millis();
    timerEnd = 1000*timeout + timerStart;
    while(1) {
        if(suli_uart_readable(serialSIM900,-1)) {
            char c = suli_uart_read_byte(serialSIM900,-1);
            sum = (c==resp[sum]) ? sum+1 : 0;
            if(sum == len)break;
        }
        if(suli_millis() > timerEnd) {
            return false;
        }
    }
    return true;    
}
*/

int sim900_wait_for_resp(const char* resp, DataType type, unsigned int timeout, unsigned int chartimeout)
{
    int len = strlen(resp);
    int sum = 0;
    //unsigned long timerStart,timerEnd, prevChar;    //prevChar is the time when the previous Char has been read.
    unsigned long timerStart, prevChar;    //prevChar is the time when the previous Char has been read.
    timerStart = suli_millis();
    prevChar = 0;
//    timerEnd = 1000*timeout + timerStart;
    while(1) {
        if(suli_uart_readable(serialSIM900,-1)) {
            char c = suli_uart_read_byte(serialSIM900,-1);
            prevChar = suli_millis();
            sum = (c==resp[sum]) ? sum+1 : 0;
            if(sum == len)break;
        }
        if ((unsigned long) (suli_millis() - timerStart) > timeout * 1000UL) {
        //if(suli_millis() > timerEnd) {
            return -1;
        }
        //If interchar Timeout => return FALSE. So we can return sooner from this function.
        if (((unsigned long) (suli_millis() - prevChar) > chartimeout) && (prevChar != 0)) {
            return -2;
        }
        
    }
    //If is a CMD, we will finish to read buffer.
    if(type == CMD) sim900_flush_serial();
    return 0;   
}

void sim900_send_End_Mark(void)
{
    suli_uart_send_byte(serialSIM900,-1,(char)26);
}

int sim900_check_with_cmd(const char* cmd, const char *resp, DataType type, unsigned int timeout, unsigned int chartimeout)
{
    sim900_send_cmd(cmd);
    return sim900_wait_for_resp(resp,type,timeout,chartimeout);
}

//FUTURE esto vale para algo en cadenas de texto tan cortas ???
/*int sim900_check_with_cmd(const __FlashStringHelper* cmd, const char *resp, unsigned timeout, DataType type)
{
    sim900_send_cmd(cmd);
    return sim900_wait_for_resp(resp,timeout,type);
}*/

