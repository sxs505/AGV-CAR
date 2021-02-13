#include "led.h"

int Led_Count;//LED��˸����
/**************************************************************************
�������ܣ�LED�ӿڳ�ʼ��
��ڲ������� 
����  ֵ����
**************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //ʹ�ܶ˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = LED_PIN;				//�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50M
	GPIO_Init(LED_PORT, &GPIO_InitStructure);			//�����趨������ʼ��GPIO
	GPIO_SetBits(LED_PORT,LED_PIN);
}

void Buzzer_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ�ܶ˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = Buzzer_PIN;			//�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	//50M
	GPIO_Init(Buzzer_PORT, &GPIO_InitStructure);		//�����趨������ʼ��GPIO
	GPIO_ResetBits(Buzzer_PORT,Buzzer_PIN);
}
/**************************************************************************
�������ܣ�LED����˸����
��ڲ������� 
����  ֵ����
**************************************************************************/
void led_task(void *pvParameters)
{
    while(1)
    {
      LED=~LED;        //LED״̬ȡ����0�ǵ�����1��Ϩ��                                  
      vTaskDelay(Led_Count); //LED��˸����ǳ��򵥣���Ƶ�ʾ���Ҫ��ͣ�ʹ�������ʱ����
    }
}  

/**************************************************************************
*  �������ܣ�LED��˸
*
*  ��ڲ�����time����˸Ƶ��
*
*  �� �� ֵ����
**************************************************************************/
void Led_Flash(u16 time)
{
	  static int temp;
	  if(0==time) LED=0;
	  else		if(++temp==time)	LED=~LED,temp=0;
}
