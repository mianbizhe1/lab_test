#include "wifi_function.h"
#include "wifi_config.h"
#include "SysTick.h"
#include <string.h> 
#include <stdio.h>  
#include <stdbool.h>


#include <stdint.h>



// ���嵥����ID������Ϊ0���������ʵ�����
#define Single_ID 0 


char esp8266_response[256];     //
volatile uint8_t response_received = 0;


// ���ESP8266���ݻ�����
void ClearEsp8266Buffer(void) {
    // 1. ������ݻ�����������0��
    memset(strEsp8266_Fram_Record.Data_RX_BUF, 0, RX_BUF_MAX_LEN);
    
    // 2. ���ó��Ⱥͱ�־λ�����ַ�ʽ���ɣ�
    strEsp8266_Fram_Record.InfAll = 0;  // ���帳ֵ���Ƽ���
    // �򵥶����ã�
    // strEsp8266_Fram_Record.InfBit.FramLength = 0;
    // strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;
}

/*
 * ��������ESP8266_Choose
 * ����  ��ʹ��/����WF-ESP8266ģ��
 * ����  ��enumChoose = ENABLE��ʹ��ģ��
 *         enumChoose = DISABLE������ģ��
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_Choose ( FunctionalState enumChoose )
{
	if ( enumChoose == ENABLE )
		ESP8266_CH_HIGH_LEVEL();
	
	else
		ESP8266_CH_LOW_LEVEL();
	
}


/*
 * ��������ESP8266_Rst
 * ����  ������WF-ESP8266ģ��
 * ����  ����
 * ����  : ��
 * ����  ����ESP8266_AT_Test����
 */
void ESP8266_Rst ( void )
{
	#if 0
	 ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
	
	#else
	 ESP8266_RST_LOW_LEVEL();
	 delay_ms ( 500 ); 
	 ESP8266_RST_HIGH_LEVEL();
	 
	#endif

}


/*
 * ��������ESP8266_AT_Test
 * ����  ����WF-ESP8266ģ�����AT��������
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_AT_Test ( void )
{
	ESP8266_RST_HIGH_LEVEL();
	
	delay_ms ( 1000 ); 
	
	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, 200 ) ) ESP8266_Rst ();  	

}


/*
 * ��������ESP8266_Cmd
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	//strEsp8266_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�
	ClearEsp8266Buffer(); // ���ESP8266���ݻ�����

	ESP8266_Usart ( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
		return true;
	
	delay_ms ( waittime );                 //��ʱ
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}



bool ESP8266_Cmd_Json ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	//strEsp8266_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�
	ClearEsp8266Buffer(); // ���ESP8266���ݻ�����

	ESP8266_Usar_Json ( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
		return true;
	
	delay_ms ( waittime );                 //��ʱ
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}


/*
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
		
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
		
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
		
	  default:
		  return false;
  }
	
}


/*
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 7000 );
	
}


/*
 * ��������ESP8266_BuildAP
 * ����  ��WF-ESP8266ģ�鴴��WiFi�ȵ�
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 *       ��enunPsdMode��WiFi���ܷ�ʽ�����ַ���
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_BuildAP ( char * pSSID, char * pPassWord, char * enunPsdMode )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode );
	
	return ESP8266_Cmd ( cCmd, "OK", 0, 1000 );
	
}


/*
 * ��������ESP8266_Enable_MUSART3_printfultipleId
 * ����  ��WF-ESP8266ģ������������
 * ����  ��enumEnUnvarnishTx�������Ƿ������
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}


/*
 * ��������ESP8266_Link_Server
 * ����  ��WF-ESP8266ģ�������ⲿ������
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
	  sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
	
}




/*
 * ESP8266_Link_MQTT
 * ����  ��WF-ESP8266ģ�������ⲿ������
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Link_MQTT (  char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

	sprintf ( cStr, "\"%s\",%s",  ip, ComNum );
    sprintf ( cCmd, "AT+MQTTCONN=0,%s,0", cStr );
	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
	
}




/*
 * ESP8266_Set_MQTT_User
 * ����  ��set MQTT user

 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Set_MQTT_User ()
{
	char cStr [100] = { 0 }, cCmd [120];
    // init  cCmd's value :AT+MQTTUSERCFG=0,1,"testUser4002","user4","1234",0,0,""
	sprintf ( cCmd, "AT+MQTTUSERCFG=0,1,\"testUser4002\",\"user4\",\"1234\",0,0,\"\"" );
	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
}


/*
 * ESP8266_Set_MQTT_public
 * ����  ��set MQTT public

 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Set_MQTT_Public (char * topicId ,char * val)
{
	char cStr [100] = { 0 }, cCmd [120];
     //construct string: AT+MQTTPUB=0,"topic911","66693",0,0
	//sprintf ( cCmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0", topicId, val );
	sprintf ( cCmd, "AT+MQTTPUB=0,\"topic911\",\"{\\\"aa\\\":\\\"01\\\"}\",1,0");

	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
}

/*
 * ESP8266_MQTT_Public_key_Value
 * ����  ��set MQTT public key and value
 * ����  ��topicId������ID�ַ���
 * 	  ��key�����ַ���
 * 	  ��val��ֵ�ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */


bool ESP8266_MQTT_Public_key_Value (char * topicId ,char * key,char * val )
{
	char cStr [100] = { 0 }, cCmd [120];
     //construct string: AT+MQTTPUB=0,"topic911","66693",0,0
	//sprintf ( cCmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0", topicId, val );
	sprintf ( cCmd, "AT+MQTTPUB=0,\"%s\",\"{\\\"%s\\\":\\\"%s\\\"}\",1,0",topicId, key, val );
	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
}


bool ESP8266_Set_MQTT_Public_JSON (char * topicId ,char * val)
{
	char cStr [100] = { 0 }, cCmd [120];
     //construct string: AT+MQTTPUB=0,"topic911","66693",0,0
	sprintf ( cCmd, "AT+MQTTPUB=0,\"topic911\",\"{\"aa\":\"01\"}\",1,0 " );
	// set cCmd's value is :
	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
}


//su  esp8266 sub mqtt topic function ��parameter is topicId & qos  ,include  parameter declaration 
bool ESP8266_Set_MQTT_Sub (const  char * topicId,const  char * qos)
{
	char cStr [100] = { 0 }, cCmd [120];
	//construct string: AT+MQTTSUB=0,"topic911",1,0
	sprintf ( cCmd, "AT+MQTTSUB=0,\"%s\",%s", topicId, qos );
	PC_Usart("send sub cmd: %s\r\n", cCmd); // �����־���
	return ESP8266_Cmd (cCmd, "OK", "ALREAY CONNECT", 500);
}







/*
 * ��������ESP8266_StartOrShutServer
 * ����  ��WF-ESP8266ģ�鿪����رշ�����ģʽ
 * ����  ��enumMode������/�ر�
 *       ��pPortNum���������˿ں��ַ���
 *       ��pTimeOver����������ʱʱ���ַ�������λ����
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

		return ( ESP8266_Cmd ( cCmd1, "OK", 0, 500 ) &&
						 ESP8266_Cmd ( cCmd2, "OK", 0, 500 ) );
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );

		return ESP8266_Cmd ( cCmd1, "OK", 0, 500 );
	}
	
}


/*
 * ��������ESP8266_UnvarnishSend
 * ����  ������WF-ESP8266ģ�����͸������
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_UnvarnishSend ( void )
{
	return (
	  ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ) &&
	  ESP8266_Cmd ( "AT+CIPSEND", "\r\n", ">", 500 ) );
	
}


/*
 * ��������ESP8266_SendString
 * ����  ��WF-ESP8266ģ�鷢���ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 *       ��pStr��Ҫ���͵��ַ���
 *       ��ulStrLength��Ҫ���͵��ַ������ֽ���
 *       ��ucId���ĸ�ID���͵��ַ���
 * ����  : 1�����ͳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
		
	if ( enumEnUnvarnishTx )
		ESP8266_Usart ( "%s", pStr );

	
	else
	{
		// if ( ucId < 5 )
		// 	sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		// else
		// 	sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Cmd ( cStr, "> ", 0, 1000 );

		bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 1000 );
  }
	
	return bRet;

}


/*
 * ��������ESP8266_ReceiveString
 * ����  ��WF-ESP8266ģ������ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 * ����  : ���յ����ַ����׵�ַ
 * ����  �����ⲿ����
 */
char * ESP8266_ReceiveString ( FunctionalState enumEnUnvarnishTx )
{
	char * pRecStr = 0;
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	while ( ! strEsp8266_Fram_Record .InfBit .FramFinishFlag );
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
	
	if ( enumEnUnvarnishTx )
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, ">" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}
	
	else 
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}

	return pRecStr;
	
}


/*
 * ��������ESP8266_STA_TCP_Client
 * ����  ��PZ-ESP8266ģ�����STA TCP Clien����
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_STA_TCP_Client ( void )
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [2], * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

	ESP8266_Choose ( ENABLE );	

  ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( STA );
  
	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 5000 );
		
  do
	{
		//PC_Usart ( "\r\n������Ҫ���ӵ�WiFi���ƺ���Կ�������ʽΪ�������ַ�+Ӣ�Ķ���+��Կ�ַ�+�ո񣬵������\r\n" );
         // write english 		PC_Usart ( "\r\n������Ҫ���ӵ�WiFi���ƺ���Կ�������ʽΪ�������ַ�+Ӣ�Ķ���+��Կ�ַ�+�ո񣬵������\r\n" );
		PC_Usart ( "\r\nPlease input the WiFi name and password to connect, format: SSID+comma+password+space, then click send\r\n" );
		scanf ( "%s", cStrInput );

		// write english PC_Usart ( "\r\n�Ե�Ƭ�� ����\r\n" );
		PC_Usart ( "\r\nPlease wait a moment...\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ESP8266_JoinAP ( pStrDelimiter [0], pStrDelimiter [1] ) );
	
	ESP8266_Enable_MultipleId ( ENABLE );
	
	do 
	{
	// write english	PC_Usart ( "\r\n���ڵ����Ͻ��������������TCP Server�������磬��������Ե�IP�Ͷ˿ںţ������ʽΪ������IP+Ӣ�Ķ���+�˿ں�+�ո񣬵������\r\n" );
		PC_Usart ( "\r\nPlease connect the network debugging assistant to the TCP Server on your computer, and input the computer's IP and port number, format: IP+comma+port+space, then click send\r\n" );

		scanf ( "%s", cStrInput );

	// write english		PC_Usart ( "\r\n�Ե�Ƭ�� ����\r\n" );
		

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ( ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_0 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_1 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_2 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_3 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_4 ) ) );

  for ( uc = 0; uc < 5; uc ++ )
	{
		// write english	 PC_Usart ( "\r\n������˿�ID%dҪ���͵��ַ����������ʽΪ���ַ����������ո�+�ո񣬵������\r\n", uc );
		PC_Usart ( "\r\nPlease input the string to send for port ID%d, format: string (no spaces) + space, then click send\r\n", uc );

		scanf ( "%s", cStrInput );

		ul = strlen ( cStrInput );
		
		ESP8266_SendString ( DISABLE, cStrInput, ul, ( ENUM_ID_NO_TypeDef ) uc );
		
	}
	
	
	// write english PC_Usart ( "\r\n��������������ַ����ַ���\r\n" );
	PC_Usart ( "\r\nPlease send strings in the network debugging assistant\r\n" );
	while (1)
	{
	  pStr = ESP8266_ReceiveString ( DISABLE );
		PC_Usart ( "%s", pStr );
	}

}





/*
 * ��������ESP8266_STA_TCP_Client_Single
 * ����  ��ESP8266ģ�����STA TCP Client���ԣ�������ģʽ��
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */

// ������Ҫ��ͷ�ļ��Ѿ�����������ʾ���������õ�һЩ


void ESP8266_STA_TCP_Client_Single ( void )
{
    // �̶�����
    const char* ssid = "sjc";                
    const char* password = "meng216216";        
    const char* server_ip = "192.168.46.226";
    const char* server_port = "8080";        
    
    // **�����忪ͷ���������б���**
    u8 wifi_retry = 0;
    bool wifi_connected = false;
    u8 server_retry = 0;
    bool server_connected = false;
    const char* test_data;
    u32 data_len;

    // ����ģ��
    PC_Usart("\r\nInitializing ESP8266...\r\n");
    ESP8266_Choose(ENABLE);
    ESP8266_AT_Test();
    
    // ����STAģʽ
    PC_Usart("\r\nConnecting to WiFi: %s...\r\n", ssid);
    ESP8266_Net_Mode_Choose(STA);
    
    // WiFi����ѭ�����������ڿ�ͷ�����������ظ���
    while (wifi_retry < 3 && !wifi_connected) {
        wifi_connected = ESP8266_JoinAP((char*)ssid, (char*)password);
        if (!wifi_connected) {
            PC_Usart("\r\nWiFi connection failed, retrying %d/3...\r\n", wifi_retry+1);
            wifi_retry++;
            delay_ms(1000);
        }
    }
    
    // ������ģʽ
    //ESP8266_Enable_MultipleId(DISABLE); // �رն�����
    
    // ���ӷ�����
    PC_Usart("\r\nConnecting to server: %s:%s...\r\n", server_ip, server_port);
    while (server_retry < 3 && !server_connected) {
        // ǿ��ת��ö������Ϊuint8_t������������Ϊuint8_t��
        server_connected = ESP8266_Link_Server((uint8_t)enumTCP, (char*)server_ip, (char*)server_port, 9); // ����Single_ID=0
        if (!server_connected) {
            PC_Usart("\r\nServer connection failed, retrying %d/3...\r\n", server_retry+1);
            server_retry++;
            delay_ms(1000);
        }
    }
    
    // �������ݣ��������ڿ�ͷ������
    test_data = "Hello from ESP8266!";
    data_len = strlen(test_data);
    PC_Usart("\r\nSending test data: %s\r\n", test_data);
    if (ESP8266_SendString(DISABLE, (char*)test_data, data_len, 0)) { // ����Single_ID=0
        PC_Usart("\r\nData sent successfully!\r\n");
    } else {
        PC_Usart("\r\nFailed to send data!\r\n");
    }
    
    // ����ѭ��
    while (1) { /* ... */ }
}





/*
 * ��������ESP8266_STA_TCP_Client_MQTT
 * ����  ��ESP8266ģ�����STA TCP Client���ԣ�������ģʽ��
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */

// ������Ҫ��ͷ�ļ��Ѿ�����������ʾ���������õ�һЩ


void ESP8266_STA_TCP_Client_MQTT ( void )
{
    // �̶�����
    const char* ssid = "sjc";                
    const char* password = "meng216216";        
    const char* server_ip = "192.168.46.226";
    const char* server_port = "1883";        
    
    // **�����忪ͷ���������б���**
    u8 wifi_retry = 0;
    bool wifi_connected = false;
    u8 server_retry = 0;
    bool server_connected = false;
    u8 setUser_retry = 0;   // ��������û������Դ���
    bool setUser_mqtt = false;  // ��������û�������״̬
	u8 setPublic_retry = 0;   // ������÷��������Դ���
	bool setPublic_mqtt = false;  // ������÷���������״̬
	// sub mqtt topic retry
	u8 sub_retry = 0;   // ��Ӷ��ĵ����Դ���
	bool sub_mqtt = false;  // ��Ӷ��ĵ�����״̬
	//sub topic id
	const char* topic_id = "topic911"; // ���趩�ĵ�����IDΪtopic911 
	// sub qos
	char qos[] = "0"; // �������޸ĵ��ַ�����
	// �������ݣ��������ڿ�ͷ������



    const char* test_data;
    u32 data_len;

    // ����ģ��
    PC_Usart("\r\nInitializing ESP8266...\r\n");
    ESP8266_Choose(ENABLE);
    ESP8266_AT_Test();
    
    // ����STAģʽ
    PC_Usart("\r\nConnecting to WiFi: %s...\r\n", ssid);
    ESP8266_Net_Mode_Choose(STA);
    
    // WiFi����ѭ�����������ڿ�ͷ�����������ظ���
    while (wifi_retry < 3 && !wifi_connected) {
        wifi_connected = ESP8266_JoinAP((char*)ssid, (char*)password);
        if (!wifi_connected) {
            PC_Usart("\r\nWiFi connection failed, retrying %d/3...\r\n", wifi_retry+1);
            wifi_retry++;
            delay_ms(1000);
        }
    }


// ��������� ��Ӧ�÷ŵ���ʼ������......

    
    // ������ģʽ
    //ESP8266_Enable_MultipleId(DISABLE); // �رն�����

    //setUser_mqtt  in while loop  3 times
	PC_Usart("\r\nSetting MQTT user...\r\n");
	while (setUser_retry < 3 && !setUser_mqtt) {
		setUser_mqtt = ESP8266_Set_MQTT_User();
		if (!setUser_mqtt) {
			PC_Usart("\r\nSetting MQTT user failed, retrying %d/3...\r\n", setUser_retry+1);
			setUser_retry++;
			delay_ms(1000);
		}
	}

	// link mqtt server
	PC_Usart("\r\nConnecting to MQTT server...\r\n");
	while (server_retry < 3 && !server_connected) {
		// ǿ��ת��ö������Ϊuint8_t������������Ϊuint8_t��
		server_connected = ESP8266_Link_MQTT((char*)server_ip, (char*)server_port, 9); // ����Single_ID=0
		if (!server_connected) {
			PC_Usart("\r\nMQTT server connection failed, retrying %d/3...\r\n", server_retry+1);
			server_retry++;
			delay_ms(1000);
		}
	}


// setPublic_mqtt  in while loop  1 time
	// PC_Usart("\r\n MQTT public topic...\r\n");
	// while (setPublic_retry < 3 && !setPublic_mqtt) {
	// 	char topicId[] = "topic911";
	// 	char val[] = "{status: 1}"; // ����JSONʾ��
	// 	setPublic_mqtt = ESP8266_Set_MQTT_Public(topicId, val);
	// 	if (!setPublic_mqtt) {
	// 		PC_Usart("\r\nSetting MQTT public topic failed, retrying %d/3...\r\n", setPublic_retry+1);
	// 		setPublic_retry++;
	// 		delay_ms(1000);
	// 	}
	// }


	// sub mqtt topic
	// PC_Usart("\r\nSubscribing to MQTT topic...\r\n");
	// while (sub_retry < 3 && !sub_mqtt) {
	// 	sub_mqtt = ESP8266_Set_MQTT_Sub(topic_id,qos); // ����qosΪ1
	// 	if (!sub_mqtt) {
	// 		PC_Usart("\r\nSubscribing to MQTT topic failed, retrying %d/3...\r\n", sub_retry+1);
	// 		sub_retry++;
	// 		delay_ms(1000);



	// 	}
	// }



    
    // �������ݣ��������ڿ�ͷ������
    // test_data = "Hello from ESP8266!";
    // data_len = strlen(test_data);
    // PC_Usart("\r\nSending test data: %s\r\n", test_data);
    // if (ESP8266_SendString(DISABLE, (char*)test_data, data_len, 0)) { // ����Single_ID=0
    //     PC_Usart("\r\nData sent successfully!\r\n");
    // } else {
    //     PC_Usart("\r\nFailed to send data!\r\n");
    // }
    
    // ����ѭ��
	// while (1) {

	// }
}


// public mqtt topic message  function,parameter is topicId & val
bool ESP8266_Public_MQTT_Message (const char * topicId, const char * val)
{
	char cStr [100] = { 0 }, cCmd [120];
	// construct string: AT+MQTTPUB=0,"topic911","66693",0,0
	sprintf ( cCmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0", topicId, val );
	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
}	






/*
 * ��������ESP8266_AP_TCP_Server
 * ����  ��PZ-ESP8266ģ�����AP TCP Server����
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_AP_TCP_Server ( void )
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

  ESP8266_Choose ( ENABLE );

	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( AP );


	PC_Usart ( "\r\n������Ҫ������WiFi�����ơ����ܷ�ʽ����Կ�����ܷ�ʽ�ı��Ϊ��\
              \r\n0 = OPEN\
              \r\n1 = WEP\
              \r\n2 = WPA_PSK\
	            \r\n3 = WPA2_PSK\
              \r\n4 = WPA_WPA2_PSK\
							\r\n�����ʽΪ�������ַ�+Ӣ�Ķ���+���ܷ�ʽ���+Ӣ�Ķ���+��Կ�ַ�+�ո񣬵������\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n�Ե�Ƭ�� ����\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP ( pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1] );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 ); //*
		

	ESP8266_Enable_MultipleId ( ENABLE );
		
	
	PC_Usart ( "\r\n�����������Ҫ�����Ķ˿ںźͳ�ʱʱ�䣨0~28800����λ���룩�������ʽΪ���˿ں��ַ�+Ӣ�Ķ���+��ʱʱ���ַ�+�ո񣬵������\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n�Ե�Ƭ�� ����\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer ( ENABLE, pStrDelimiter [0], pStrDelimiter [1] );
	
	
	do
	{
		PC_Usart ( "\r\n����ѯ��ģ��IP����\r\n" );
	  ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 500 );
		
		PC_Usart ( "\r\n�����ֻ����ӸղŴ�����WiFi������ֻ����һ���ֻ�����ΪID0��Ȼ�����ֻ��������������TCP Client���Ӹղſ����ķ�������AP IP������\r\n" );
		delay_ms ( 20000 ) ;
	}	while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:0", 0, 500 ) );
	

	PC_Usart ( "\r\n������Ҫ��˿��ֻ���ID0�����͵��ַ����������ʽΪ���ַ����������ո�+�ո񣬵������\r\n" );

	scanf ( "%s", cStrInput );

	ul = strlen ( cStrInput );
	
	ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_0 );

	
	PC_Usart ( "\r\n�����ֻ�����������ַ����ַ���\r\n" );
	while (1)
	{
	  pStr = ESP8266_ReceiveString ( DISABLE );
		PC_Usart ( "%s", pStr );
	}

}


/*
 * ��������ESP8266_StaTcpClient_ApTcpServer
 * ����  ��PZ-ESP8266ģ�����STA(TCP Client)+AP(TCP Server)����
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_StaTcpClient_ApTcpServer ( void )
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

  ESP8266_Choose ( ENABLE );

	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( STA_AP );


	PC_Usart ( "\r\n������Ҫ������WiFi�����ơ����ܷ�ʽ����Կ�����ܷ�ʽ�ı��Ϊ��\
						\r\n0 = OPEN\
						\r\n1  =WEP\
						\r\n2 = WPA_PSK\
						\r\n3 = WPA2_PSK\
						\r\n4 = WPA_WPA2_PSK\
						\r\n�����ʽΪ�������ַ�+Ӣ�Ķ���+���ܷ�ʽ���+Ӣ�Ķ���+��Կ�ַ�+�ո񣬵������\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n�Ե�Ƭ�� ����\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP ( pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1] );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 ); //*
	

	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 5000 );
		
  do
	{
		PC_Usart ( "\r\n������Ҫ���ӵ�WiFi���ƺ���Կ�������ʽΪ�������ַ�+Ӣ�Ķ���+��Կ�ַ�+�ո񣬵������\r\n" );

		scanf ( "%s", cStrInput );

		PC_Usart ( "\r\n�Ե�Ƭ�� ����\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ESP8266_JoinAP ( pStrDelimiter [0], pStrDelimiter [1] ) );

	
	ESP8266_Enable_MultipleId ( ENABLE );
		
	
	PC_Usart ( "\r\n�����������Ҫ�����Ķ˿ںźͳ�ʱʱ�䣨0~28800����λ���룩�������ʽΪ���˿ں��ַ�+Ӣ�Ķ���+��ʱʱ���ַ�+�ո񣬵������\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n�Ե�Ƭ�� ����\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer ( ENABLE, pStrDelimiter [0], pStrDelimiter [1] );
	
	
	do 
	{
		PC_Usart ( "\r\n���ڵ����Ͻ��������������TCP Server�������磬��������Ե�IP�Ͷ˿ںţ������ʽΪ������IP+Ӣ�Ķ���+�˿ں�+�ո񣬵������\r\n" );

		scanf ( "%s", cStrInput );

		PC_Usart ( "\r\n�Ե�Ƭ�� ����\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ( ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_0 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_1 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_2 ) ) );
		
	
	do
	{
		PC_Usart ( "\r\n����ѯ��ģ��IP��ǰһ��ΪAP IP����һ��ΪSTA IP����\r\n" );
	  ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 500 );
		
		PC_Usart ( "\r\n�����ֻ����ӸղŴ�����WiFi������ֻ����һ���ֻ�����ΪID3��Ȼ�����ֻ��������������TCP Client���Ӹղſ����ķ�������AP IP������\r\n" );
		delay_ms ( 20000 ) ;
	}	while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:3", 0, 500 ) );
	

	for ( uc = 0; uc < 3; uc ++ )
	{
		PC_Usart ( "\r\n������˿�ID%dҪ���͵��ַ����������ʽΪ���ַ����������ո�+�ո񣬵������\r\n", uc );

		scanf ( "%s", cStrInput );

		ul = strlen ( cStrInput );
		
		ESP8266_SendString ( DISABLE, cStrInput, ul, ( ENUM_ID_NO_TypeDef ) uc );
		
	}
	
	
	PC_Usart ( "\r\n������Ҫ��˿��ֻ���ID3�����͵��ַ����������ʽΪ���ַ����������ո�+�ո񣬵������\r\n" );

	scanf ( "%s", cStrInput );

	ul = strlen ( cStrInput );
	
	ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_3 );

	
	PC_Usart ( "\r\n���ڵ�������������ֻ��ֻ�����������ַ����ַ�������\r\n" );
	while (1)
	{
	  pStr = ESP8266_ReceiveString ( DISABLE );
		PC_Usart ( "%s", pStr );
	}
	
}






