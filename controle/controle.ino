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
	
	/* INICIANDO RECEPTOR RADIO FREQUENCIA */
	mySwitch.enableReceive(RF_RECEPTOR);
	
	/* ABRINDO SISTEMA DE ARQUIVOS DA CENTRAL */
	openFS();
	
	/* INICIALIZANDO ATUALIZADOR REMOTO DO FIRMWARE */
	ota_update_remote();

	Serial.println();
	Serial.println("****** SISTEMA INICIADO ******");
	Serial.println();
	
	/* FUNÇÃO QUE LE ARQUIVO DE CONFIGURAÇÃO QUE CONTEM OS CONTROLES CADASTRADO NA CENTRAL */
	arq_jason();
  
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
      Serial.print(" # V_DEC_RETORNO - "+String(V_DEC_RETORNO));
      Serial.println(" | V_DECIMAL - "+String(V_DECIMAL));
  		
			/* CONDIÇÃO PARA QUE EXIBA A LEITURA E ARMAZENAR O CONTROLE (BOTÃO) APENAS 1 VEZ NA SERIAL*/
  		if ((String(V_DEC_RETORNO) != String(V_DECIMAL)) && (ct != 3))
  		{
        BOTAO_PRESS += String(V_DECIMAL);
        Serial.println(" # Senha: "+ BOTAO_PRESS);
        ct++;
        V_DEC_RETORNO = V_DECIMAL;
        if(BOTAO_PRESS == SENHA)
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

