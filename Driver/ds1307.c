#include <ds1307.h>
#define EEPROM_ADDRESS 0x50
#define I2C_SPEED 100000
#define Time_out 10000					//ms

#define I2C_SCL GPIO_Pin_10
#define I2C_SDA GPIO_Pin_11

#define I2C_PORT GPIOB

#define WRITE 0x00
#define READ 0x01


void __I2C_Init(void)
{
   I2C_InitTypeDef  I2C_InitStructure;
   GPIO_InitTypeDef  GPIO_InitStructure; 

   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);
	
   GPIO_InitStructure.GPIO_Pin =  I2C_SCL | I2C_SDA;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
   GPIO_Init(I2C_PORT, &GPIO_InitStructure);
		
   I2C_DeInit(I2C2);
   I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
   I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
   I2C_InitStructure.I2C_OwnAddress1 = EEPROM_ADDRESS;
   I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
   I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;  
    
   I2C_Cmd(I2C2, ENABLE);
   I2C_Init(I2C2, &I2C_InitStructure);
   I2C_AcknowledgeConfig(I2C2, ENABLE);
}

static void I2C_AcknowledgePolling()
{
	vu16 SR1_Tmp;
	do
	{   
	I2C_GenerateSTART(I2C2, ENABLE);
	SR1_Tmp = I2C_ReadRegister(I2C2, I2C_Register_SR1);
		I2C_Send7bitAddress(I2C2, (EEPROM_ADDRESS<<1)|1, I2C_Direction_Transmitter);
	}
	while(!(I2C_ReadRegister(I2C2, I2C_Register_SR1) & 0x0002));
	I2C_ClearFlag(I2C2, I2C_FLAG_AF);
	I2C_GenerateSTOP(I2C2, ENABLE);
}
void EEPROM_Write_Byte(unsigned int Address, unsigned char Byte)
{
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); 
	I2C_Send7bitAddress(I2C2, (EEPROM_ADDRESS<<1)| WRITE, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2, (unsigned char)(Address>>8));
	I2C_SendData(I2C2, (unsigned char)Address);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	
	I2C_SendData(I2C2, Byte); 
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C2, ENABLE);
	I2C_AcknowledgePolling();
}
void EEPROM_Write(unsigned int Address, unsigned char *Data, unsigned int Length)
{
	unsigned int i = 0;
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); 
	I2C_Send7bitAddress(I2C2, (EEPROM_ADDRESS<<1) | WRITE, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2, (unsigned char)(Address>>8));
	I2C_SendData(I2C2, (unsigned char)Address);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	for (i = 0; i < Length; i++)
		I2C_SendData(I2C2, Data[i]); 
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C2, ENABLE);
	I2C_AcknowledgePolling();
}
unsigned char EEPROM_Read_Byte(unsigned char Address)
{
	unsigned int Buffer = 0;
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,  (EEPROM_ADDRESS << 1) | WRITE, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2, (unsigned char)(Address>>8));
	I2C_SendData(I2C2, (unsigned char)Address);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2, (EEPROM_ADDRESS<<1) | READ, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
	Buffer = I2C_ReceiveData(I2C2);
	I2C_AcknowledgeConfig(I2C2, DISABLE);	
	I2C_GenerateSTOP(I2C2, ENABLE);		
	return Buffer;
}
void EEPROM_Read(unsigned char Address, unsigned char *Data, unsigned int Length)
{
	Data[0] = EEPROM_Read_Byte(Address);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,  (EEPROM_ADDRESS << 1) | READ, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	Data++;
	Length--;
	while(Length--)
	{
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
		*Data = I2C_ReceiveData(I2C2);
		Data++;
	}
	I2C_AcknowledgeConfig(I2C2, DISABLE);	
	I2C_GenerateSTOP(I2C2, ENABLE);		
}

