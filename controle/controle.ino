#include <RCSwitch.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#define RF_RECEPTOR 			13
#define BT_GRAVA_CONTROL 	35
#define LED_PROCESSO 			13

const char* CHAVE_BIN_CONTROLE, *CONTROLE_1, *CONTROLE_2, *CONTROLE_3, *CONTROLE_4, *CONTROLE_5, *CONTROLE_6;
int COUNT_LER_JSON = 0;

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
	
  Serial.println("INICIADO");
}

void loop() 
{
	ArduinoOTA.handle();
	while (COUNT_LER_JSON < 1)
  { 
    StaticJsonDocument<700> doc;
    const char *json = lerArquivo("LISTA_CONTROLES.txt").c_str();
    DeserializationError error = deserializeJson(doc, json);
    JsonObject root = doc.as<JsonObject>();
		CONTROLE_1      = root["CONTROLE_1"];
		Serial.println("Controle 1: "+String(CONTROLE_1));
		CONTROLE_2      = root["CONTROLE_2"];
		Serial.println("Controle 2: "+String(CONTROLE_2));
		CONTROLE_3      = root["CONTROLE_3"];
		Serial.println("Controle 3: "+String(CONTROLE_3));
		CONTROLE_4      = root["CONTROLE_4"];
		Serial.println("Controle 4: "+String(CONTROLE_4));
		CONTROLE_5      = root["CONTROLE_5"];
		Serial.println("Controle 5: "+String(CONTROLE_5));
		CONTROLE_6      = root["CONTROLE_6"];
		Serial.println("Controle 6: "+String(CONTROLE_6));
    COUNT_LER_JSON++;
  }
  if (mySwitch.available()) 
	{
    output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
    CHAVE_BIN_CONTROLE = binario(mySwitch.getReceivedValue(),  mySwitch.getReceivedBitlength());
		mySwitch.resetAvailable();
  }
	if(BT_GRAVA_CONTROL == true)
	{
		//gravarArquivo("{\"CONTROLE_1\":\""+CHAVE_BIN_CONTROLE+"\",}", "LISTA_CONTROLES.txt");
		//gravarArquivo(CHAVE_BIN_CONTROLE, CHAVE_BIN_CONTROLE+".txt");
		
	}
}
