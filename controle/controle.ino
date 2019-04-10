#include <RCSwitch.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#define RF_RECEPTOR 			13
#define BT_GRAVA_CONTROL 	35
#define LED_PROCESSO 			2
#define RL_SAIDA_1				14

int COUNT_LER_JSON = 0, V_DECIMAL, V_DEC_RETORNO = 0;
int DISPOSITIVO_RF[60][4];
const char* LISTA_DISP_RF[] = {"1", "2", "3"};
const char* CONTROLES = "{\"1\":[121212,232323,121212,232323],\"2\":[454545,545454,414141,252525],\"3\":[363636,474747,959595,616161]}";

RCSwitch mySwitch = RCSwitch();

void setup()
{
	Serial.begin(115200);
	pinMode(LED_PROCESSO,OUTPUT);
	digitalWrite(LED_PROCESSO,LOW);
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
}

void loop()
{
	ArduinoOTA.handle();
	if (mySwitch.available())
	{
		//output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
		
		/* FUNÇÃO QUE FAZ A LEITURA DO CONTROLE REMOTO, EXIBE VALOR EM DECIMAL*/
		V_DECIMAL = mySwitch.getReceivedValue();
		
		/* CONDIÇÃO PARA QUE EXIBA A LEITURA DO CONTROLE APENAS 1 VEZ NA SERIAL*/
		if (V_DEC_RETORNO != V_DECIMAL)
		{
			Serial.println(V_DECIMAL);
			V_DEC_RETORNO = V_DECIMAL;
			digitalWrite(LED_PROCESSO,!digitalRead(LED_PROCESSO));
			digitalWrite(RL_SAIDA_1,!digitalRead(RL_SAIDA_1));
		}
		mySwitch.resetAvailable();
	}
}
