#include <buzzer.h>

unsigned int PWM_Value;
static void Timer_Calculate(unsigned long Hz, unsigned int *TimerPeriod, unsigned int *Prescale)
{
	unsigned int Du = 0;
	unsigned char Restrict = 0;
	*Prescale = 0;
	*TimerPeriod = 0;
	do
	{
		(*Prescale)++;
		*TimerPeriod = SystemCoreClock / (((*Prescale) + 1)*Hz) - 1;
		Du = SystemCoreClock % ((*Prescale + 1)*Hz);
		if ((*Prescale) == 0xffff)
		{
			Restrict = 1;
			*Prescale = 0;
			*TimerPeriod = 0;
			break;
		}
	} while ((*TimerPeriod) > 65535 || Du != 0 || Restrict);
}
static unsigned int TSVN_PWM_TIM2_Init(unsigned long Hz)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	unsigned int TimerPeriod;
	unsigned int PrescaleValue;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO , ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = BUZZER_CHANNEL_1_PIN;
	GPIO_Init(BUZZER_CHANNEL_PORT, &GPIO_InitStructure);
	
	Timer_Calculate(Hz, &TimerPeriod, &PrescaleValue);
	
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescaleValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//Setduty
	TIM_OCInitStructure.TIM_Pulse = 0;					//Set duty
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);			
	return TimerPeriod;
	
}
static void TSVN_PWM_TIM2_Set_Duty(unsigned int Ratio)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//Setduty
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = Ratio;		
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
}
static void TSVN_PWM_TIM2_Start(void)
{
	TIM_Cmd(TIM2, ENABLE);
}
static void TSVN_PWM_TIM2_Stop(void)
{
	TIM_Cmd(TIM2, DISABLE);
}

void Buzzer_Init(void)
{
	 PWM_Value = TSVN_PWM_TIM2_Init(2000);
}
void Buzzer_SetDuty(unsigned int percent)
{
	unsigned long tmpValue = (unsigned long)((float)percent*PWM_Value/100.0);
	TSVN_PWM_TIM2_Set_Duty((unsigned int)tmpValue);
}
void Buzzer_SetFreq(unsigned long freq)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	unsigned int TimerPeriod;
	unsigned int PrescaleValue;
	
	Timer_Calculate(freq, &TimerPeriod, &PrescaleValue);
	
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescaleValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	PWM_Value = TimerPeriod;
}

void Buzzer_Start(void)
{
	TSVN_PWM_TIM2_Start();
}
void Buzzer_Stop(void)
{
	TSVN_PWM_TIM2_Stop();
}
