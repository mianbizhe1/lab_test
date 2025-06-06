#include "system.h"
#include "SysTick.h"
#include "led.h"         // ���LED��������ͷ�ļ�
#include "usart.h"
#include "wifi_config.h"
#include "wifi_function.h"
#include <string.h>
#include "cJSON.h"
#include "dht11.h"




// �����ⲿ����
extern struct STRUCT_USARTx_Fram strEsp8266_Fram_Record;

// ��������
void ProcessWiFiData(uint8_t *data, uint16_t length);
void HandleMQTTMessage(const char *topic, const char *message, const char *message_le);

// ȫ�ֱ���
char topic_id[50] = "mqtt_env_demo/demo_001/report";
char topic_id2[50] = "mqtt_env_demo/demo_001/report2";
char sub_topic_id[20] = "topic108";
uint8_t wifi_rx_buffer[RX_BUF_MAX_LEN]; // �����ݴ�������ݵĻ�����
//delcare global variable for if opening and closing read data
bool read_data_flag = true; // ���ڿ����Ƿ��ȡ����
// ========== �豸����ṹ�嶨�� ==========
typedef enum {
    DEVICE_LED,
    DEVICE_MOTOR,
    DEVICE_UNKNOWN
} DeviceType;

typedef struct {
    const char *device_name;        // �豸���ƣ�JSON������
    DeviceType device_type;         // �豸����ö��
    void (*handler)(const char *state); // ������ָ��
} DeviceHandler;



void data_pros()	//���ݴ�����
{
	u8 temp;  	    
	u8 humi;
	u8 temp_buf[3],humi_buf[3];
	DHT11_Read_Data(&temp,&humi);
	temp_buf[0]=temp/10+0x30;	
	temp_buf[1]=temp%10+0x30;
	temp_buf[2]='\0';
	//LCD_ShowString(55,100,tftlcd_data.width,tftlcd_data.height,16,temp_buf);
		
	humi_buf[0]=humi/10+0x30;	
	humi_buf[1]=humi%10+0x30;
	humi_buf[2]='\0';
	//LCD_ShowString(55,130,tftlcd_data.width,tftlcd_data.height,16,humi_buf);
	printf("temperature=%d��C  humidity=%d%%RH\r\n",temp,humi);
   // publish data to MQTT topic
    if (ESP8266_MQTT_Public_key_Value(topic_id, "temp", temp_buf)) {
        printf("Published temperature successfully!\r\n");
    } else {
        printf("Failed to publish temperature!\r\n");
    }
    
    if (ESP8266_MQTT_Public_key_Value(topic_id2, "shidu", humi_buf)) {
        printf("Published humidity successfully!\r\n");
    } else {
        printf("Failed to publish humidity!\r\n");
    }

}


// ========== �豸������ʵ�� ==========
// LED������������LED_Init()����led.h��������
void LedHandler(const char *state) {
    if (strcmp(state, "on") == 0) {
        LED_ON(); // ����LED_ON()��led.c��ʵ��
        printf("Led turned on\r\n");
    } else if (strcmp(state, "off") == 0) {
        LED_OFF(); // ����LED_OFF()��led.c��ʵ��
        printf("Led turned off\r\n");
    } else {
        printf("Invalid state for LED: %s\r\n", state);
    }
}

// Motor������
void MotorHandler(const char *state) {
    if (strcmp(state, "on") == 0) {
        // ������ӵ����������
        TIM_SetCompare2(TIM3,400); // ����ʹ��TIM3ͨ��2���Ƶ�� ����
        printf("Motor started\r\n");
    } else if (strcmp(state, "off") == 0) {
        // ������ӵ���رմ���
        TIM_SetCompare2(TIM3,0); // ����ʹ��TIM3ͨ��2���Ƶ����ֹͣ���
        printf("Motor stopped\r\n");
    } else {
        printf("Invalid state for Motor: %s\r\n", state);
    }
}

// �豸����ע�������չ��
DeviceHandler device_handlers[] = {
    {"Led", DEVICE_LED, LedHandler},
    {"Motor", DEVICE_MOTOR, MotorHandler},
    {NULL, DEVICE_UNKNOWN, NULL} // ������־
};

// ========== JSON����������չ ==========
void parse_json_message(const char *message) {
    cJSON *root;
    cJSON *item; // ����������������δʹ�þ���

    if (message == NULL) {
        printf("����:�����JSON�ַ���Ϊ��\n");
        return;
    }

    root = cJSON_Parse(message);
    if (!root) {
        printf("JSON��ʽ����:%s\n", cJSON_GetErrorPtr());
        return;
    }

    // ����JSON�����еļ�ֵ�ԣ�������������λ�ã�
    item = root->child; // �����������ڴ���鿪ͷ���˴�ֱ��ʹ��
    while (item) {
        const char *device_name = item->string;
        const char *state = item->valuestring;
        
        // �����豸������
        DeviceHandler *handler = device_handlers;
        while (handler->device_name) {
            if (strcmp(device_name, handler->device_name) == 0) {
                handler->handler(state);
                break;
            }
            handler++;
        }

        if (!handler->device_name) { // δ�ҵ�������
            printf("Unknown device: %s\r\n", device_name);
        }
        item = item->next;
    }

    cJSON_Delete(root); // �ͷ��ڴ�
}

int main() {
    // ��ʼ�����ֱ��ֲ���...
    SysTick_Init(72);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    USART1_Init(115200);
    LED_Init(); // ����LED��ʼ��������led.c��
    // Motor_Init(); // ���е����ʼ�������

    WiFi_Config();

    printf("Hello, world!\r\n");
    printf("\r\n====================================\r\n");
    printf("STM32 + ESP8266 JSON Command Handler\r\n");
    printf("Supported devices: Led, Motor\r\n");
    printf("====================================\r\n");

    // ��ʼ��WiFi��MQTT
    ESP8266_STA_TCP_Client_MQTT();
    if (ESP8266_Set_MQTT_Sub(sub_topic_id, "0")) {
        printf("\r\nSubscribed to MQTT topic successfully!\r\n");
    } else {
        printf("\r\nFailed to subscribe to MQTT topic!\r\n");
    }

    if (ESP8266_MQTT_Public_key_Value(topic_id, "aa", "01")) {
        printf("\r\nPublished MQTT message successfully!\r\n");
    } else {
        printf("\r\nFailed to publish MQTT message!\r\n");
    }


	while(DHT11_Init())	//���DS18B20�Ƿ���
	{
		//LCD_ShowString(130,50,tftlcd_data.width,tftlcd_data.height,16,"Error   ");
		printf("DHT11 Check Error!\r\n");
		delay_ms(500);		
	}
	//LCD_ShowString(130,50,tftlcd_data.width,tftlcd_data.height,16,"Success");
	printf("DHT11 Check OK!\r\n");
    TIM3_CH2_PWM_Init(500,72-1); //Ƶ����2Kh
	TIM_SetCompare2(TIM3,0);   //PWM�����ʼֵΪ0  ���Ӧ����ֹͣ״̬


    while (1) {
        // ����WiFi����
        if (strEsp8266_Fram_Record.InfBit.FramFinishFlag) {
            uint16_t data_len; // �����ڴ���鿪ͷ
            __disable_irq();   // �����ٽ���
            data_len = strEsp8266_Fram_Record.InfBit.FramLength;

            // �������ݵ����ػ�����
            memcpy(wifi_rx_buffer, strEsp8266_Fram_Record.Data_RX_BUF, data_len);
            wifi_rx_buffer[data_len] = '\0'; // ����ַ���������

            // �����־�ͼ�����
            strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;
            strEsp8266_Fram_Record.InfBit.FramLength = 0;
            __enable_irq(); // �˳��ٽ���

            // ������յ�������
            ProcessWiFiData(wifi_rx_buffer, data_len);
        }
        // ����ҵ���߼�...
        //��ʪ��
        if(read_data_flag) { // ���������ȡ���ݱ�־
            printf("\r\nReading DHT11 data...\r\n");
        	data_pros();  	 //��ȡһ��DHT11��������Ҫ����100m
        } 

        delay_ms(200);
    }
    // ��ӻ��з����⾯��
    printf("\n");
}

// ����WiFi���ݵĺ���
void ProcessWiFiData(uint8_t *data, uint16_t length) {
    // ʾ����ֱ�Ӵ�ӡԭʼ����
    printf("[RAW DATA] %s\r\n", data);

    // ����Ƿ���MQTT��Ϣ
    if (strstr((char *)data, "+MQTTSUBRECV:") != NULL) {
        // ����MQTT��Ϣ
        char *topic_start = strchr((char *)data, ',') + 1;
        char *msg_length_start = strchr(topic_start, ',') + 1;
        char *msg_start = strchr(msg_length_start, ',') + 1;

        // ��ȡ�������Ϣ
        char topic[32] = {0};
        char msg_length[32] = {0};
        char message[128] = {0};

        sscanf(topic_start, "%[^,]", topic);
        sscanf(msg_length_start, "%[^,]", msg_length);
        sscanf(msg_start, "%[^,]", message);
        HandleMQTTMessage(topic, message, msg_length);
    }
    // �����������Э�鴦��...
}

// ����MQTT��Ϣ
void HandleMQTTMessage(const char *topic, const char *message, const char *message_length) {
    printf("Received MQTT message:\r\n");
    printf("Topic: %s\r\n", topic);         // topic_id
    printf("Message: %s\r\n\r\n", message); // json �ַ���
    // todo: ����ʵ����������Ϣ
    parse_json_message(message); // ����JSON��Ϣ
}
// ��ӻ��з����⾯��