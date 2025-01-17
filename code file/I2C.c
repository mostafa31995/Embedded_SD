﻿

/*
 * I2C.c
 *
 * Created: 29/10/2016 02:07:51 م
 *  Author: LENOVO
 */ 

#include <avr/io.h>
#include "I2C.h"
#include "UART.h"


// bit rate:18 (freq: 100Khz @8MHz)
//************************************************
void twi_init(void)
{
 TWCR= 0x00; //disable twi
 TWBR= 0x12; //set bit rate
 TWSR= 0x00; //set prescale
 //TWCR= 0x44; //enable twi
}
void i2c_init(u32 scl_frequency,u32 clock)
{
	unsigned int twi_value;
	// Initial ATMega328P TWI/I2C Peripheral
	TWSR = 0x02;         // Select Prescaler of  _16

	twi_value = ((clock/ scl_frequency)-16)/(2*16);
	TWBR = (twi_value<<0) ;
}

//*************************************************
//Function to start i2c communication
//*************************************************
unsigned char i2c_start(void)
{
 
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); 	//Send START condition
	
    while (!(TWCR & (1<<TWINT)));   		//Wait for TWINT flag set. This indicates that the
		  								//START condition has been transmitted
    if ((TWSR & 0xF8) == START)			//Check value of TWI Status Register
 	   return(0);
	else
	   return(1);
}

//*************************************************
//Function for repeat start condition
//*************************************************
unsigned char i2c_repeatStart(void)
{
 
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); 		//Send START condition
    while (!(TWCR & (1<<TWINT)));   		//Wait for TWINT flag set. This indicates that the
		  								//START condition has been transmitted
    if ((TWSR & 0xF8) == REPEAT_START)			//Check value of TWI Status Register
 	   return(0);
	else
	   return(1);
}
//**************************************************
//Function to transmit address of the slave
//*************************************************
unsigned char i2c_sendAddress(unsigned char address)
{
   unsigned char STATUS;
   
   if((address & 0x01) == 0) 
     STATUS = MT_SLA_ACK;
   else
     STATUS = MR_SLA_ACK; 
   
   TWDR = address; 
   TWCR = (1<<TWINT)|(1<<TWEN);	   //Load SLA_W into TWDR Register. Clear TWINT bit
   		  			 				   //in TWCR to start transmission of address
   while (!(TWCR & (1<<TWINT)));	   //Wait for TWINT flag set. This indicates that the
   		 		   					   //SLA+W has been transmitted, and
									   //ACK/NACK has been received.
   if ((TWSR & 0xF8) == STATUS)	   //Check value of TWI Status Register
   	  return(0);
   else 
      return(1);
}

//**************************************************
//Function to transmit a data byte
//*************************************************
unsigned char i2c_sendData(unsigned char data)
{
   TWDR = data; 
   TWCR = (1<<TWINT) |(1<<TWEN);	   //Load SLA_W into TWDR Register. Clear TWINT bit
   		  			 				   //in TWCR to start transmission of data
   while (!(TWCR & (1<<TWINT)));	   //Wait for TWINT flag set. This indicates that the
   		 		   					   //data has been transmitted, and
									   //ACK/NACK has been received.
   if ((TWSR & 0xF8) != MT_DATA_ACK)   //Check value of TWI Status Register
   	  return(1);
   else
      return(0);
}

//*****************************************************
//Function to receive a data byte and send ACKnowledge
//*****************************************************
unsigned char i2c_receiveData_ACK(void)
{
  unsigned char data;
  
  TWCR = (1<<TWEA)|(1<<TWINT)|(1<<TWEN);
  
  while (!(TWCR & (1<<TWINT)));	   	   //Wait for TWINT flag set. This indicates that the
   		 		   					   //data has been received
  if ((TWSR & 0xF8) != MR_DATA_ACK)    //Check value of TWI Status Register
   	  return(ERROR_CODE);
  
  data = TWDR;
  return(data);
}

//******************************************************************
//Function to receive the last data byte (no acknowledge from master
//******************************************************************
unsigned char i2c_receiveData_NACK(void)
{
  unsigned char data;
  
  TWCR = (1<<TWINT)|(1<<TWEN);
  
  while (!(TWCR & (1<<TWINT)));	   	   //Wait for TWINT flag set. This indicates that the
   		 		   					   //data has been received
  if ((TWSR & 0xF8) != MR_DATA_NACK)    //Check value of TWI Status Register
   	  return(ERROR_CODE);
  
  data = TWDR;
  return(data);
}
//**************************************************
//Function to end the i2c communication
//*************************************************   	
void i2c_stop(void)
{
  TWCR =  (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);	  //Transmit STOP condition
}  