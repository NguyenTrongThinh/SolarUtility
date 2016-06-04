#include <bluetooth.h>

#define TSVN_USART_PORT GPIOA

#define TSVN_USART_RX_PIN GPIO_Pin_3
#define TSVN_USART_TX_PIN GPIO_Pin_2
#define BLUETOOTH_STATE GPIO_Pin_13

#define TSVN_USART_BAURATE 115200

#define BLUETOOTH_PORT  USART2

#define START 								0xFE
#define END									0xFF


Command_t ReceiveData;

void Bluetooth_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = TSVN_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TSVN_USART_PORT, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = TSVN_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(TSVN_USART_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = TSVN_USART_BAURATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(BLUETOOTH_PORT, &USART_InitStructure);
	USART_ITConfig(BLUETOOTH_PORT, USART_IT_RXNE, ENABLE);
	USART_ITConfig(BLUETOOTH_PORT, USART_IT_TXE, DISABLE);
	USART_ClearITPendingBit(BLUETOOTH_PORT, USART_IT_RXNE);
	USART_Cmd(BLUETOOTH_PORT, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin   =  BLUETOOTH_STATE;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_IPU;
	GPIO_Init(TSVN_USART_PORT, &GPIO_InitStructure);
	
}
void Bluetooth_Deinit(void)
{
	USART_Cmd(BLUETOOTH_PORT, DISABLE);
}
bool Bluetooth_connect(void)
{
	unsigned char status = GPIO_ReadInputDataBit(TSVN_USART_PORT, BLUETOOTH_STATE);
	return (status == 1)?true:false;
}

bool Bluetooth_GetData(unsigned char Data)
{
	bool Finished = false;
	switch(Data)
	{
		case START:
		
			memset(ReceiveData.Command, 0, sizeof(ReceiveData.Command));
			ReceiveData.Length = 0;
			ReceiveData.Command[ReceiveData.Length++] = START;
		
			break;
		case END: 
		
			ReceiveData.Command[ReceiveData.Length++] = END;
			Finished = true;						
		
			break;
		default: 
			ReceiveData.Command[ReceiveData.Length++] = Data;
			break;
	}
	return Finished;
}
void Bluetooth_GetCommand(Command_t *data)
{
	memcpy(data, &ReceiveData, sizeof(ReceiveData));
}
bool Bluetooth_ReceiveValidData(Command_t Data)
{
	unsigned char i = 0;
	unsigned char checksum = 0;
	for (i = 0; i < Data.Length - 2; i++)
	{
		checksum ^= Data.Command[i];
	}
	checksum ^= Data.Command[Data.Length - 1];
	if (checksum == Data.Command[Data.Length - 2])
		return true;
	return false;
}
int fputc(int ch, FILE *f)
{
  USART_SendData(USART2, (uint8_t) ch);
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}
  return ch;
}
