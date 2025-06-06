#include "led.h"

/*******************************************************************************
* �� �� ��         : LED_Init
* ��������		   : LED��ʼ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//����ṹ�����
	
	RCC_APB2PeriphClockCmd(LED1_PORT_RCC|LED2_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=LED1_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(LED1_PORT,&GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	GPIO_SetBits(LED1_PORT,LED1_PIN);   //��LED�˿����ߣ�Ϩ������LED
	
	GPIO_InitStructure.GPIO_Pin=LED2_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_Init(LED2_PORT,&GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	GPIO_SetBits(LED2_PORT,LED2_PIN);   //��LED�˿����ߣ�Ϩ������LED
}

// void LED_ON(void) declaration
void LED_ON(void)
{
	GPIO_ResetBits(LED1_PORT, LED1_PIN); // ����LED1
	GPIO_ResetBits(LED2_PORT, LED2_PIN); // ����LED2
}
// void LED_OFF(void) declaration
void LED_OFF(void)
{
	GPIO_SetBits(LED1_PORT, LED1_PIN); // Ϩ��LED1
	GPIO_SetBits(LED2_PORT, LED2_PIN); // Ϩ��LED2
}

bool LED1_Status(void)
{
   // ���LED1״̬
	return GPIO_ReadOutputDataBit(LED1_PORT, LED1_PIN) == Bit_RESET; // ����ǵ͵�ƽ����ʾLED1��
}

bool LED2_Status(void)
{
	// ���LED2״̬
	return GPIO_ReadOutputDataBit(LED2_PORT, LED2_PIN) == Bit_RESET; // ����ǵ͵�ƽ����ʾLED2��
   
}
