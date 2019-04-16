int arq_jason()
{
  StaticJsonDocument<700> DOC;
  DeserializationError ERR =  deserializeJson (DOC,CONTROLES);
  if(ERR)
  {
     Serial.print("ERROR: ");
     Serial.print(ERR.c_str());
  }
  JsonObject BOTOES = DOC.as<JsonObject>();
  int c;
  int cc;
  while(c < sizeof(LISTA_DISP_RF)/4 )
  {
    Serial.println("CONTROLE: "+String(c)); 
    for(int i=0; i < 4; i++)
      {
        DISPOSITIVO_RF[cc]= BOTOES[LISTA_DISP_RF[c]][i];
        Serial.print(" BOTAO: "+String(DISPOSITIVO_RF[cc]));
        cc++;
      }
     Serial.println();
    c++;
  }
  Serial.println();
}
void ota_update_remote(){
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
}

int search_Array(int ARRAY[], int VALOR)
{
  int POSICAO;
  int i = 0;
  int x = 0;
  Serial.println(" # Verificando se controle esta cadastrado na central");
  while((DISPOSITIVO_RF[i] != NULL) && (x == 0)) 
  {
    if (DISPOSITIVO_RF[i] == VALOR)
    {
      POSICAO = i;
      Serial.println(" # Achou o botão "+String(VALOR)+" na posição: "+String(POSICAO));
      x = 1;
      }else
      {
        i++;
        POSICAO = NULL;
      }
    
  }
  return POSICAO;
}

String quebraString(String txtMsg,String string)
{ 
	unsigned int tamanho 	= txtMsg.length();
	int inicio_string 		= string.indexOf(txtMsg)+tamanho+1;
	int final_string 			= string.indexOf("&",inicio_string);
	String resultado 			= string.substring(inicio_string, final_string);
	//Serial.println(resultado);
	return resultado;
} 
/* ---------------------------------------  
   FUNÃ‡ÃƒO PARA ACIONAMENTO DE PORTAS 
   GPIO
---------------------------------------   */
void aciona_Porta(int NUMERO, String ACAO) {
	//gravaLog(" "+hora_ntp + " - Comando:"+String(NUMERO)+"/"+ACAO, logtxt, 4);
	String BUF;
	if (ACAO == "liga") {
		digitalWrite(NUMERO, HIGH );
		//BUF = "porta="+String(NUMERO)+"&acao=liga&central="+ipLocalString;
		//gravarBanco(BUF);
		BUF = "";
	}else if (ACAO == "desl") {
		digitalWrite(NUMERO, LOW);
		//BUF = "porta="+String(NUMERO)+"&acao=desliga&central="+ipLocalString;
		//gravarBanco(BUF);
		BUF = "";
	}else if (ACAO == "puls"){
		//BUF = "porta="+String(NUMERO)+"&acao=pulso&central="+ipLocalString;
		//gravarBanco(BUF);
		digitalWrite(NUMERO, HIGH);
		delay(1000);
		digitalWrite(NUMERO, LOW);
		BUF = "";
	}
} 
//String teste_conexao(){
//	WiFiClient client = server.available();
//	if(millis() >= time3+time3Param)
//	{
//		time3 = millis();
//		int r = client.connect(servidor, portaServidor);
//		if(r == 0)
//		{
//			////gravaLog(" "+hora_ntp + " - ERRO 0104 - Servidor WEB ou Banco de Dados Desconectado", logtxt, 1);
//			retorno = "ERRO_SERVIDOR_DESC";
//		}else if(r == 1)
//		{
//			Serial.println(" Servidor WEB/Banco OK ");
//			retorno = "SERVIDOR_CONECT";
//		}
//	}
//	return retorno;
//}
//
////---------------------------------------  
////    FUNÃ‡ÃƒO PARA GRAVAR NO BANCO
////---------------------------------------  
//void gravarBanco (String buffer){
//	WiFiClient client = server.available();
//	if(WiFi.status() != WL_CONNECTED)
//	{
//		////gravaLog(" "+hora_ntp + " - ERRO 0105 - NÃ£o foi possivel conectar ao servidor WEB ou banco de dados, reiniciando a central!", logtxt, 1);
//		WiFi.reconnect();
//		if(WiFi.status() != WL_CONNECTED){
//			pisca_led(LED_VERDE,false);
//			pisca_led(LED_VERMELHO,true);
//			////gravaLog(" "+hora_ntp + " - Falha ao conectar ao WIFI e atingir o tempo limite", logtxt, 1);
//			//ESP.restart();
//			delay(1000);
//		} 
//	}
//	pisca_led(LED_VERMELHO,false);
//	if ((client.connect(servidor, portaServidor) == true) && (teste_conexao() == "SERVIDOR_CONECT")) 
//	{
//		//if (client.connect(servidor, 80)) {
//		client.println("GET /web/gravar.php?"+buffer);
//		////gravaLog(" "+hora_ntp + " - Gravando no banco de dados: "+buffer, logtxt, 4);
//		client.println();
//		buffer = "";
//	} else {
//		////gravaLog(" "+hora_ntp + " - ERRO 0104 - Servidor WEB ou Banco de Dados Desconectado...:", logtxt, 1);
//		buffer = "";
//	}
//	client.flush();
//	client.stop();
//}
////---------------------------------------  
//---------------------------------------
//    FUNÃ‡ÃƒO DE MANIPULAÃ‡ÃƒO DE ARQUIVOS  
//---------------------------------------  
void formatFS(){
	SPIFFS.format();
}

void criarArquivo(){
	File wFile;
	//Cria o arquivo se ele nÃ£o existir
	if(SPIFFS.exists("/log.txt")){
		Serial.println(" Arquivo log.txt ja existe!");
	} else {
		Serial.println(" Criando o arquivo log.txt...");
		wFile = SPIFFS.open("/log.txt","w+");
		//Verifica a criaÃ§Ã£o do arquivo
		if(!wFile){
		} else {
			Serial.println(" Arquivo log.txt criado com sucesso!");
		}
	}
	if(SPIFFS.exists("/param.txt")){
		Serial.println(" Arquivo 'param.txt' ja existe!");
	} else {
		Serial.println(" Criando o arquivo 'param.txt'...");
		wFile = SPIFFS.open("/param.txt","w+");
		//Verifica a criaÃ§Ã£o do arquivo
		if(!wFile){
			////gravaLog(" "+hora_ntp + " - ERRO 0109 - Erro ao criar arquivo 'param.txt'", logtxt, 1);
		} else {
			Serial.println(" Arquivo 'param.txt' criado com sucesso!");
		}
	}
	wFile.close();
}
void deletarArquivo(String arquivo) {
	//Remove o arquivo
	if(!SPIFFS.remove(arquivo)){
		////gravaLog(" "+hora_ntp + " - ERRO 0105 - Erro ao remover arquivo "+arquivo, logtxt, 1);
	} else {
		Serial.println(" Arquivo "+arquivo+" removido com sucesso!");
	}
}
void gravarArquivo(String msg, String arq) {
	//Abre o arquivo para adiÃ§Ã£o (append)
	//Inclue sempre a escrita na ultima linha do arquivo
	if(arq == "log.txt")
	{
		File logg = SPIFFS.open("/log.txt","a+");
		int s = logg.size(); // verificar tamanho do arquivo
		if (s >= 10000){
			deletarArquivo("/log.txt");
			criarArquivo(); 
			delay(1000);
			////gravaLog(" "+hora_ntp + " - Log foi deletado por estar grande demais! ", logtxt, 1);
		}
		if(!logg){
			//Gravando log de erro na central.
			Serial.println(" ERRO 0108 - Erro ao abrir arquivo "+arq);
		} else {
			logg.println(msg);
			Serial.println(msg);
		}
		logg.close();
	}
	
	if(arq == "param.txt")
	{
		File param1 = SPIFFS.open("/param.txt","a+");
		if(!param1){
			////gravaLog(" "+hora_ntp + " - ERRO 0106 - Erro ao abrir arquivo "+arq, logtxt, 1);
		} else {
			param1.println(msg);
			////gravaLog(" "+hora_ntp + " - Gravando novos paramentos: "+msg, logtxt, 1);
		}
		param1.close();
	}
}

String lerArquivo(String nome) {
	//Faz a leitura do arquivo
	String buff;
	File ARQUIVO = SPIFFS.open("/"+nome,"r");
	if(!ARQUIVO){
		SPIFFS.open("/"+nome,"w+");
	}
	while(ARQUIVO.available()) {
		String line = ARQUIVO.readStringUntil('\n');
		buff += line+"<br />";
	}
	ARQUIVO.close();
	return buff;
}

String lerArquivoParam(void) {
	String buff;
	File ARQUIVO = SPIFFS.open("/param.txt","r");
	//Serial.println("Lendo Parametros da central...");
	while(ARQUIVO.available()) {
		String line = ARQUIVO.readStringUntil('\n');
		buff += line;
	}
	ARQUIVO.close();
	return buff;
}
void closeFS(){
	SPIFFS.end();
}
void openFS(){
	//Abre o sistema de arquivos
	SPIFFS.begin();
	if(!SPIFFS.begin()){
		////gravaLog(" "+hora_ntp + " - ERRO 0107 - Erro ao abrir sistema de arquivo interno da central! ", logtxt, 1);
	} else {
		////gravaLog(" "+hora_ntp + " - Sistema de arquivos aberto com sucesso!", logtxt, 4);
	}
}