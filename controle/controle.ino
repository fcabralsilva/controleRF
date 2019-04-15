#include <RCSwitch.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#define RF_RECEPTOR 			13
#define BT_GRAVA_CONTROL 	35
#define LED_OK            2
#define RL_SAIDA_1				14

long int SENHA = 679784867978426797844;         

int COUNT_LER_JSON = 0, V_DECIMAL, V_DEC_RETORNO = 1111111, ct;
String BOTAO_PRESS;
int DISPOSITIVO_RF[60];
const char* LISTA_DISP_RF[] = {"1", "2", "3"};
const char* CONTROLES = "{\"1\":[6797848,6797844,6797842,179623333],\"2\":[454545,545454,414141,252525],\"3\":[363636,474747,959595,616161]}";

RCSwitch mySwitch = RCSwitch();

void setup()
{
	Serial.begin(115200);
	pinMode(LED_OK,OUTPUT);
	digitalWrite(LED_OK,LOW);
	pinMode(RL_SAIDA_1,OUTPUT);
	digitalWrite(RL_SAIDA_1,LOW);
	pinMode(BT_GRAVA_CONTROL,INPUT_PULLUP);
	
	mySwitch.enableReceive(RF_RECEPTOR);
	openFS();

	ArduinoOTA
	.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH)
		type = "sketch";
		else // U_SPIFFS
		type = "filesystem";

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	})
	.onEnd([]() {
		Serial.println("\nEnd");
	})
	.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	})
	.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();
	Serial.println();
	Serial.println("****** SISTEMA INICIADO ******");
	Serial.println();
	
	/* FUNÇÃO QUE LE ARQUIVO DE CONFIGURAÇÃO QUE CONTEM OS CONTROLES
		CADASTRADO NA CENTRAL*/
	arq_jason();
  //search_Array(DISPOSITIVO_RF, 179623333);
}

void loop()
{
	ArduinoOTA.handle();
	if (mySwitch.available())
	{
		//output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
		
		/* FUNÇÃO QUE FAZ A LEITURA DO CONTROLE REMOTO, EXIBE VALOR EM DECIMAL*/
		V_DECIMAL = mySwitch.getReceivedValue();
    if(search_Array(DISPOSITIVO_RF,V_DECIMAL) != NULL)
    {
      Serial.print("V_DEC_RETORNO - "+String(V_DEC_RETORNO));
      Serial.println(" | V_DECIMAL - "+String(V_DECIMAL));
  		/* CONDIÇÃO PARA QUE EXIBA A LEITURA DO CONTROLE APENAS 1 VEZ NA SERIAL*/
  		if ((V_DEC_RETORNO != V_DECIMAL) && (ct != 3))
  		{
        BOTAO_PRESS += String(V_DECIMAL);
        Serial.println(" Senha: "+ BOTAO_PRESS);
        ct++;
        V_DEC_RETORNO = V_DECIMAL;
        if(V_DEC_RETORNO == SENHA)
        {
    			digitalWrite(LED_OK,!digitalRead(LED_OK));
    			digitalWrite(RL_SAIDA_1,!digitalRead(RL_SAIDA_1));
          delay(1000);
          digitalWrite(RL_SAIDA_1,!digitalRead(RL_SAIDA_1));
          digitalWrite(LED_OK,!digitalRead(LED_OK));
          V_DEC_RETORNO = 0;
          ct = 3;
        }
  		}
    }
		mySwitch.resetAvailable();
	}
}
int search_Array(int ARRAY[], int VALOR)
{
  int POSICAO;
  int i = 0;
  int x = 0;
  Serial.println(" #Verificando se controle esta cadastrado na central");
  while((DISPOSITIVO_RF[i] != NULL) && (x == 0)) 
  {
    if (DISPOSITIVO_RF[i] == VALOR)
    {
      POSICAO = i;
      Serial.println(" #Achou o botão "+String(VALOR)+" na posição: "+String(POSICAO));
      x = 1;
      }else
      {
        i++;
        POSICAO = NULL;
      }
    
  }
  return POSICAO;
}
