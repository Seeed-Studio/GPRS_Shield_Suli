/*
* gprs.cpp 
* A library for SeeedStudio seeeduino GPRS shield 
*  
* Copyright (c) 2013 seeed technology inc. 
* Author      	: 	lawliet.zou(lawliet.zou@gmail.com)
* Create Time	: 	Dec 23, 2013 
* Change Log 	: 	
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

#include <stdio.h>
#include "GPRS_Shield_Arduino.h"
#include <Suli.h>

GPRS* GPRS::inst;

GPRS::GPRS(PIN_T tx, PIN_T rx, uint32_t baudRate, const char* apn, const char* userName, const char* passWord):gprsSerial(tx,rx)
{
    inst = this;
    _apn = apn;
    _userName = userName;
    _passWord = passWord;
	sim900_init(&gprsSerial, -1, baudRate);
}

int GPRS::init(void)
{
	if(0 != sim900_check_with_cmd("AT\r\n","OK",DEFAULT_TIMEOUT,CMD)){
		return -1;
	}
	
	if(0 != sim900_check_with_cmd("AT+CFUN=1\r\n","OK",DEFAULT_TIMEOUT,CMD)){
		return -1;
	}
	
    if(0 != checkSIMStatus()) {
        return -1;
    }
	
    return 0;
}

int GPRS::checkSIMStatus(void)
{
    char gprsBuffer[32];
    int count = 0;
    sim900_clean_buffer(gprsBuffer,32);
    while(count < 3) {
        sim900_send_cmd("AT+CPIN?\r\n");
        sim900_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
        if((NULL != strstr(gprsBuffer,"+CPIN: READY"))) {
            break;
        }
        count++;
        suli_delay_ms(300);
    }
    if(count == 3) {
        return -1;
    }
    return 0;
}



int GPRS::sendSMS(char *number, char *data)
{
	char cmd[32];
    if(0 != sim900_check_with_cmd("AT+CMGF=1\r\n", "OK", DEFAULT_TIMEOUT,CMD)) { // Set message mode to ASCII
        return -1;
    }
    suli_delay_ms(500);
    snprintf(cmd, sizeof(cmd),"AT+CMGS=\"%s\"\r\n", number);
    if(0 != sim900_check_with_cmd(cmd,">",DEFAULT_TIMEOUT,CMD)) {
        return -1;
    }
    suli_delay_ms(1000);
	sim900_send_cmd(data);
    suli_delay_ms(500);
	sim900_send_End_Mark();
	return 0;
}

int GPRS::readSMS(int messageIndex, char *message,int length)
{
	int i = 0;
    char gprsBuffer[100];
	char cmd[16];
    char *p,*s;
	
	sim900_check_with_cmd("AT+CMGF=1\r\n","OK",DEFAULT_TIMEOUT,CMD);
	suli_delay_ms(1000);
	sprintf(cmd,"AT+CMGR=%d\r\n",messageIndex);
	sim900_send_cmd(cmd);
    sim900_clean_buffer(gprsBuffer,100);
    sim900_read_buffer(gprsBuffer,100,DEFAULT_TIMEOUT);
    if(NULL != ( s = strstr(gprsBuffer,"+CMGR"))){
        if(NULL != ( s = strstr(gprsBuffer,"+32"))){
            p = s + 6;
            while((*p != '\r')&&(i < length-1)) {
                message[i++] = *(p++);
            }
            message[i] = '\0';
        }
    }
    return 0;	
}

int GPRS::deleteSMS(int index)
{
    char cmd[16];
    snprintf(cmd,sizeof(cmd),"AT+CMGD=%d\r\n",index);
    sim900_send_cmd(cmd);
    return 0;
}

int GPRS::callUp(char *number)
{
	char cmd[24];
    if(0 != sim900_check_with_cmd("AT+COLP=1\r\n","OK",DEFAULT_TIMEOUT,CMD)) {
        return -1;
    }
    suli_delay_ms(1000);
	sprintf(cmd,"ATD%s;\r\n", number);
	sim900_send_cmd(cmd);
    return 0;
}

int GPRS::answer(void)
{
    sim900_send_cmd("ATA\r\n");
    return 0;
}


bool GPRS::join()
{
    char cmd[64];
    char ipAddr[32];
    //Select multiple connection
    //sim900_check_with_cmd("AT+CIPMUX=1\r\n","OK",DEFAULT_TIMEOUT,CMD);

    //set APN
    snprintf(cmd,sizeof(cmd),"AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n",_apn,_userName,_passWord);
    sim900_check_with_cmd(cmd, "OK", DEFAULT_TIMEOUT,CMD);

    //Brings up wireless connection
    sim900_check_with_cmd("AT+CIICR\r\n","OK",DEFAULT_TIMEOUT,CMD);

    //Get local IP address
    sim900_send_cmd("AT+CIFSR\r\n");
    sim900_read_buffer(ipAddr,32,2);

    if(NULL != strstr(ipAddr,"AT+CIFSR")) {
        _ip = str_to_ip(ipAddr+12);
        if(_ip != 0) {
            return true;
        }
    }
    return false;
}

bool GPRS::connect(Protocol ptl,const char * host, int port, int timeout)
{
    char cmd[64];
    char resp[96];

    if(ptl == TCP) {
        sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",host, port);
    } else if(ptl == UDP) {
        sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%d\r\n",host, port);
    } else {
        return false;
    }
    sim900_send_cmd(cmd);
    sim900_read_buffer(resp,96,2*DEFAULT_TIMEOUT);
    if(NULL != strstr(resp,"CONNECT")) { //ALREADY CONNECT or CONNECT OK
        return true;
    }
    return false;
}

bool GPRS::gethostbyname(const char* host, uint32_t* ip)
{
    uint32_t addr = str_to_ip(host);
    char buf[17];
    snprintf(buf, sizeof(buf), "%d.%d.%d.%d", (addr>>24)&0xff, (addr>>16)&0xff, (addr>>8)&0xff, addr&0xff);
    if (strcmp(buf, host) == 0) {
        *ip = addr;
        return true;
    }
    return false;
}

bool GPRS::disconnect()
{
    sim900_send_cmd("AT+CIPSHUT\r\n");
    return true;
}

bool GPRS::is_connected(void)
{
    char resp[96];
    sim900_send_cmd("AT+CIPSTATUS\r\n");
    sim900_read_buffer(resp,sizeof(resp),DEFAULT_TIMEOUT);
    if(NULL != strstr(resp,"CONNECTED")) {
        //+CIPSTATUS: 1,0,"TCP","216.52.233.120","80","CONNECTED"
        return true;
    } else {
        //+CIPSTATUS: 1,0,"TCP","216.52.233.120","80","CLOSED"
        //+CIPSTATUS: 0,,"","","","INITIAL"
        return false;
    }
}

bool GPRS::close()
{
    // if not connected, return
    if (is_connected() == false) {
        return true;
    }
    if(0 != sim900_check_with_cmd("AT+CIPCLOSE\r\n", "CLOSE OK", DEFAULT_TIMEOUT,CMD)) {
        return false;
    }
    return true;
}

int GPRS::readable(void)
{
    return sim900_check_readable();
}

int GPRS::wait_readable(int wait_time)
{
	return sim900_wait_readable(wait_time);
}

int GPRS::wait_writeable(int req_size)
{
    return req_size+1;
}

int GPRS::send(const char * str, int len)
{
    char cmd[32];
    char resp[16];
    //suli_delay_ms(1000);
    if(len > 0){
        snprintf(cmd,sizeof(cmd),"AT+CIPSEND=%d\r\n",len);
        if(0 != sim900_check_with_cmd(cmd,">",DEFAULT_TIMEOUT,CMD)) {
            return false;
        }
        if(0 != sim900_check_with_cmd(str,"SEND OK",DEFAULT_TIMEOUT+2,DATA)) {
            return false;
        }
    }
    return len;
}

int GPRS::recv(char* buf, int len)
{
    sim900_clean_buffer(buf,len);
    sim900_read_buffer(buf,len,DEFAULT_TIMEOUT);
    return strlen(buf);
}

uint32_t GPRS::str_to_ip(const char* str)
{
    uint32_t ip = 0;
    char* p = (char*)str;
    for(int i = 0; i < 4; i++) {
        ip |= atoi(p);
        p = strchr(p, '.');
        if (p == NULL) {
            break;
        }
        ip <<= 8;
        p++;
    }
    return ip;
}

char* GPRS::getIPAddress()
{
	snprintf(ip_string, sizeof(ip_string), "%d.%d.%d.%d", (_ip>>24)&0xff,(_ip>>16)&0xff,(_ip>>8)&0xff,_ip&0xff); 
	return ip_string;
}
