#include <RCSwitch.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#define RF_RECEPTOR 			13
#define BT_GRAVA_CONTROL 	35
#define LED_PROCESSO 			2

int COUNT_LER_JSON = 0, V_DECIMAL, V_DEC_RETORNO = 0;
int DISPOSITIVO_RF[60][4];
const char* LISTA_DISP_RF[] = {"1","2","3"};
const char* CONTROLES ="{\"1\":[121212,232323,121212,232323],\"2\":[454545,545454,414141,252525],\"3\":[363636,474747,959595,616161]}";


RCSwitch mySwitch = RCSwitch();

void setup() 
{
  Serial.begin(115200);
  mySwitch.enableReceive(RF_RECEPTOR);  // Receiver on interrupt 0 => that is pin #2
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
  Serial.println("****** SISTEMA INICIADO******");
  Serial.println();
  /*  
    FUNÇÃO CONTROLES E BOTOES RF
  */
  arq_jason();
}

void loop() 
{
	ArduinoOTA.handle();
  if (mySwitch.available()) 
	{
    //output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
    //CHAVE_BIN_CONTROLE = binario(mySwitch.getReceivedValue(),  mySwitch.getReceivedBitlength());

    V_DECIMAL = mySwitch.getReceivedValue();  
    if(V_DEC_RETORNO != V_DECIMAL)
    {  
      Serial.println(V_DECIMAL);
      V_DEC_RETORNO = V_DECIMAL;
    }
		mySwitch.resetAvailable();
  }
  
	if(BT_GRAVA_CONTROL == true)
	{
				
	}
}
