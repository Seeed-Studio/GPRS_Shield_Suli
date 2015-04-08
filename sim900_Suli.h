/*
 * sim900_Suli.h 
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

#ifndef __SIM900_SULI_H__
#define __SIM900_SULI_H__

#include "Suli.h"
#include <SoftwareSerial.h>

// IDE just include this #define TRUE                1
// IDE just include this #define FALSE               0
#define DEFAULT_TIMEOUT     5   //Segundos
#define DEFAULT_INTERCHAR_TIMEOUT 1500  //miliseconds

enum DataType {
    CMD     = 0,
    DATA    = 1,
};

void  sim900_init(void * uart_device, int16 uart_num, uint32 baud);
int   sim900_check_readable();
int   sim900_wait_readable(int wait_time);
void  sim900_flush_serial();
char  sim900_read_byte(void);
int   sim900_read_buffer(char* buffer,int count,  unsigned int timeout = DEFAULT_TIMEOUT, unsigned int chartimeout = DEFAULT_INTERCHAR_TIMEOUT);
void  sim900_clean_buffer(char* buffer, int count);
void  sim900_send_cmd(const char* cmd);
void  sim900_send_cmd(const __FlashStringHelper* cmd);
void  sim900_send_cmd_P(const char* cmd);   
void  sim900_send_char(const char c);
void  sim900_send_AT(void);
void  sim900_send_End_Mark(void);
//NOT USED bool  sim900_response_cmp(uint8_t* resp, unsigned int len, unsigned int timeout);
int   sim900_wait_for_resp(const char* resp, DataType type, unsigned int timeout = DEFAULT_TIMEOUT, unsigned int chartimeout = DEFAULT_INTERCHAR_TIMEOUT);
int   sim900_check_with_cmd(const char* cmd, const char *resp, DataType type, unsigned int timeout = DEFAULT_TIMEOUT, unsigned int chartimeout = DEFAULT_INTERCHAR_TIMEOUT);
//FUTURE: use with F() macro: int   sim900_check_with_cmd(const __FlashStringHelper* cmd, const char *resp, unsigned timeout, DataType type);

#endif
