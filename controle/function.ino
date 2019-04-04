boolean portaIO(int entrada, int rele, const char* tipo,const char* modelo,char contador, boolean estado){
  String s_tipo_1 = String(tipo);
  String s_modelo_1 = String(modelo);
  estado_antes = estado;
  if (s_modelo_1 == "pulso")
  {
    if (digitalRead(entrada) == s_tipo_1.toInt())
    {
      if (nContar == 0)Serial.println(" Entrada "+String(entrada)+" - Modo pulso... ");
      while ((digitalRead(entrada) == s_tipo_1.toInt()) && (nContar <= 100) )
      {
        if (millis() >= tempo + paramTempo)
        {
          contador++;
          nContar++;
          Serial.print(contador, DEC);
          tempo = millis();
        }
      }
	  if( estado_antes == true) 
			{
				estado_atual = false;
			}else if( estado_antes == false) 
			{
				estado_atual = true;
			}
    }
  } else if (s_modelo_1 == "interruptor")
  {
    estado_atual = digitalRead(entrada);
    if (estado_atual != estado_antes )
    {
      if (nContar == 0)Serial.println(" Entrada "+String(entrada)+" - Modo interruptor... ");
      contador = 3;
    }
  }else if(s_modelo_1 == "pir")
	{
    estado_atual = digitalRead(entrada);
    if (estado_atual)
    {
      if (nContar == 0)Serial.println(" Entrada "+String(entrada)+" - Sensor Presença... ");
    }
	}
  if ((contador >= 2) && (contador <= 9))
  {
    if (nContar >= 100)
    {
			if(n == 0)
    {
      for(int i = 0; i <=0 ;i++ )
      {
        String ERRO_ENTRADA = hora_rtc + " - ERRO 0107 - Interruptor Porta IN: "+String(rele)+" Porta OUT: "+String(entrada)+" esta com erro de execução, deve usar a pagina para reiniciar";
        if ((nivel_log >= 1) || (logtxt == "sim")) gravarArquivo( ERRO_ENTRADA, "log.txt");
        n = 1;
      }
    }
  } else
    {
	String ERRO_ENTRADA = "0";
	nContar = 0;
	estado_antes = estado_atual;
	if (estado_antes == false) {
		Serial.println(" Ligando Porta "+String(entrada)+" : " + String(rele));
		contador = 0;
		acionaPorta(rele, "", "liga");
	} else {
		Serial.println(" Desligar Porta "+String(entrada)+" : " + String(rele));
		acionaPorta(rele, "", "desl");
		contador = 0;
      }
    }
  }
  return estado_antes;

}
void pisca_led(int LED,boolean estado)
{
  if(estado == true)
  {
    if (millis() - milis > interval) 
    {
      milis = millis();
      digitalWrite(LED, !digitalRead(LED));
    }
  }else
    {
      digitalWrite(LED_AZUL, LOW);
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_VERMELHO, LOW);
    }
}
void gravaLog(String mensagem, String permissao, int nivel)
{
	//Serial.println(mensagem);
	for(int i = 0; i <=0 ;i++ )
	{
		if (permissao == "sim")
		{
			if( String(nivel) <= String(nivelLog))
			{
				gravarArquivo(mensagem, "log.txt");
			} 
		}
	}
}

String selectedHTNL(const char* tipo, String comp )
{
	String select;
	String s_tipo = String(tipo);
	String s_comp = String(comp);
	if (s_tipo == comp) {
		return select = "selected";
	} else {
		return select = "";
	}
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
//---------------------------------------  
//    FUNÇÃO PARA ACIONAMENTO DE PORTAS 
//    GPIO
//---------------------------------------  
void acionaPorta(int numeroF, String portaF, String acaoF) {
	gravaLog(" "+hora_ntp + " - Comando:"+String(numeroF)+"/"+acaoF, logtxt, 4);
	if (acaoF == "liga") {
		digitalWrite(numeroF, HIGH );
		linha = "porta="+String(numeroF)+"&acao=liga&central="+ipLocalString;
		gravarBanco(linha);
		linha = "";
	}else if (acaoF == "desl") {
		digitalWrite(numeroF, LOW);
		linha = "porta="+String(numeroF)+"&acao=desliga&central="+ipLocalString;
		gravarBanco(linha);
		linha = "";
	}else if (acaoF == "puls"){
		linha = "porta="+String(numeroF)+"&acao=pulso&central="+ipLocalString;
		gravarBanco(linha);
		digitalWrite(numeroF, HIGH);
		delay(1000);
		digitalWrite(numeroF, LOW);
		linha = "";
	}
}
String teste_conexao(){
	WiFiClient client = server.available();
	if(millis() >= time3+time3Param)
	{
		time3 = millis();
		int r = client.connect(servidor, portaServidor);
		if(r == 0)
		{
			gravaLog(" "+hora_ntp + " - ERRO 0104 - Servidor WEB ou Banco de Dados Desconectado", logtxt, 1);
			retorno = "ERRO_SERVIDOR_DESC";
		}else if(r == 1)
		{
			Serial.println(" Servidor WEB/Banco OK ");
			retorno = "SERVIDOR_CONECT";
		}
	}
	return retorno;
}

//---------------------------------------  
//    FUNÇÃO PARA GRAVAR NO BANCO
//---------------------------------------  
void gravarBanco (String buffer){
	WiFiClient client = server.available();
	if(WiFi.status() != WL_CONNECTED)
	{
		gravaLog(" "+hora_ntp + " - ERRO 0105 - Não foi possivel conectar ao servidor WEB ou banco de dados, reiniciando a central!", logtxt, 1);
		WiFi.reconnect();
		if(WiFi.status() != WL_CONNECTED){
			pisca_led(LED_VERDE,false);
			pisca_led(LED_VERMELHO,true);
			gravaLog(" "+hora_ntp + " - Falha ao conectar ao WIFI e atingir o tempo limite", logtxt, 1);
			//ESP.restart();
			delay(1000);
		} 
	}
	pisca_led(LED_VERMELHO,false);
	if ((client.connect(servidor, portaServidor) == true) && (teste_conexao() == "SERVIDOR_CONECT")) 
	{
		//if (client.connect(servidor, 80)) {
		client.println("GET /web/gravar.php?"+buffer);
		gravaLog(" "+hora_ntp + " - Gravando no banco de dados: "+buffer, logtxt, 4);
		client.println();
		buffer = "";
	} else {
		gravaLog(" "+hora_ntp + " - ERRO 0104 - Servidor WEB ou Banco de Dados Desconectado...:", logtxt, 1);
		buffer = "";
	}
	client.flush();
	client.stop();
}
//---------------------------------------  


//---------------------------------------  
//    FUNÇÃO DA SIRENE
//--------------------------------------- 
void sirene(boolean valor){
	if(valor == true){
		ledcWriteTone(channel, 2000);
		delay(400);
		ledcWriteTone(channel, 1800);
		delay(400);
		ledcWriteTone(channel, 1000);
		delay(300);
		
	}else{
		ledcWriteTone(channel, 0);
	}
}
//---------------------------------------  


//---------------------------------------  
//    FUNÇOES DO MQ / SENSOR DE GAS
//---------------------------------------  
void calibrarSensor()
{
	//CALIBRACAO INCIAL DO SENSOR DE GAS
	Serial.print(" Caligrando sensor de gás");                
	Ro = MQCalibration(PIN_MQ2);                                      
	Serial.println("\n Calibrado com sucesso - 'Ro' = "+String(Ro)+" kohm"); 
}

float calcularResistencia(int tensao)   //funcao que recebe o tensao (dado cru) e calcula a resistencia efetuada pelo sensor. O sensor e a resistência de carga forma um divisor de tensão. 
{
	return (((float)VRL_VALOR*(4095-tensao)/tensao));
}

float MQCalibration(int mq_pin)   //funcao que calibra o sensor em um ambiente limpo utilizando a resistencia do sensor em ar limpo 9.83
{
	int i;
	float valor=0;
	pisca_led(LED_VERDE,false);
	for (i=0;i<ITERACOES_CALIBRACAO;i++) {    //sao adquiridas diversas amostras e calculada a media para diminuir o efeito de possiveis oscilacoes durante a calibracao
		Serial.print(".");
		valor += calcularResistencia(analogRead(mq_pin));
		delay(500);
		digitalWrite(LED_AZUL, !digitalRead(LED_AZUL));//Faz o LED piscar (inverte o estado).
	}
	digitalWrite(LED_AZUL,false);
	valor = valor/ITERACOES_CALIBRACAO;        
	valor = valor/RO_FATOR_AR_LIMPO; //o valor lido dividido pelo R0 do ar limpo resulta no R0 do ambiente
	return valor; 
}
float leitura_MQ2(int mq_pin)
{
	int i;
	float rs=0;
	pisca_led(LED_VERDE,false);
	for (i=0;i<ITERACOES_LEITURA;i++) {
		rs += calcularResistencia(analogRead(mq_pin));
		digitalWrite(LED_AZUL, !digitalRead(LED_AZUL));//Faz o LED piscar (inverte o estado).
		delay(10);
	}
	digitalWrite(LED_AZUL,false);
	rs = rs/ITERACOES_LEITURA;
	return rs;  
}
int getQuantidadeGasMQ(float rs_ro, int gas_id)
{
	if ( gas_id == 0 ) {
		return calculaGasPPM(rs_ro,LPGCurve);
	} else if ( gas_id == 1 ) {
		return calculaGasPPM(rs_ro,COCurve);
	} else if ( gas_id == 2 ) {
		return calculaGasPPM(rs_ro,SmokeCurve);
	}    

	return 0;
}
int  calculaGasPPM(float rs_ro, float *pcurve) //Rs/R0 é fornecido para calcular a concentracao em PPM do gas em questao. O calculo eh em potencia de 10 para sair da logaritmica
{
	return (pow(10,( ((log(rs_ro)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
//---------------------------------------  

//---------------------------------------
//    FUNÇÃO DE MANIPULAÇÃO DE ARQUIVOS  
//---------------------------------------  
void formatFS(){
	SPIFFS.format();
}

void criarArquivo(){
	File wFile;
	//Cria o arquivo se ele não existir
	if(SPIFFS.exists("/log.txt")){
		Serial.println(" Arquivo log.txt ja existe!");
	} else {
		Serial.println(" Criando o arquivo log.txt...");
		wFile = SPIFFS.open("/log.txt","w+");
		//Verifica a criação do arquivo
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
		//Verifica a criação do arquivo
		if(!wFile){
			gravaLog(" "+hora_ntp + " - ERRO 0109 - Erro ao criar arquivo 'param.txt'", logtxt, 1);
		} else {
			Serial.println(" Arquivo 'param.txt' criado com sucesso!");
		}
	}
	wFile.close();
}
void deletarArquivo(String arquivo) {
	//Remove o arquivo
	if(!SPIFFS.remove(arquivo)){
		gravaLog(" "+hora_ntp + " - ERRO 0105 - Erro ao remover arquivo "+arquivo, logtxt, 1);
	} else {
		Serial.println(" Arquivo "+arquivo+" removido com sucesso!");
	}
}
void gravarArquivo(String msg, String arq) {
	//Abre o arquivo para adição (append)
	//Inclue sempre a escrita na ultima linha do arquivo
	if(arq == "log.txt")
	{
		File logg = SPIFFS.open("/log.txt","a+");
		int s = logg.size(); // verificar tamanho do arquivo
		if (s >= 10000){
			deletarArquivo("/log.txt");
			criarArquivo(); 
			delay(1000);
			gravaLog(" "+hora_ntp + " - Log foi deletado por estar grande demais! ", logtxt, 1);
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
			gravaLog(" "+hora_ntp + " - ERRO 0106 - Erro ao abrir arquivo "+arq, logtxt, 1);
		} else {
			param1.println(msg);
			gravaLog(" "+hora_ntp + " - Gravando novos paramentos: "+msg, logtxt, 1);
		}
		param1.close();
	}
}

String lerArquivo(String nome) {
	//Faz a leitura do arquivo
	String buff;
	File ARQUIVO = SPIFFS.open("/"+nome,"r");
	if(!ARQUIVO){
		wFile = SPIFFS.open("/"+nome,"w+");
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
		gravaLog(" "+hora_ntp + " - ERRO 0107 - Erro ao abrir sistema de arquivo interno da central! ", logtxt, 1);
	} else {
		gravaLog(" "+hora_ntp + " - Sistema de arquivos aberto com sucesso!", logtxt, 4);
	}
}
//---------------------------------------  

//callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager *myWiFiManager) {  
	//  Serial.println("Entered config mode");
	Serial.println(" Entrou no modo de configuração ");
	Serial.println(WiFi.softAPIP()); //imprime o IP do AP
	Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede

}

//callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
	//  Serial.println("Should save config");
	Serial.println(" Configuração salva ");
	Serial.println(WiFi.softAPIP()); //imprime o IP do AP
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
	Serial.printf("Listando Diretórios: %s\r\n", dirname);

	File root = fs.open(dirname);
	if(!root){
		Serial.println("- Falha ao abrir diretório");
		return;
	}
	if(!root.isDirectory()){
		Serial.println(" - Diretório não encontrado");
		return;
	}

	File file = root.openNextFile();
	while(file){
		if(file.isDirectory()){
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if(levels){
				listDir(fs, file.name(), levels -1);
			}
		} else {
			Serial.print("  Arquivo: ");
			Serial.print(file.name());
			Serial.print("\tTamanho: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}
void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : Serial.println ("POWERON_RESET");break;          /**<1,  Vbat power on reset*/
    case 3 : Serial.println ("SW_RESET");break;               /**<3,  Software reset digital core*/
    case 4 : Serial.println ("OWDT_RESET");break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : Serial.println ("DEEPSLEEP_RESET");break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : Serial.println ("SDIO_RESET");break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : Serial.println ("TG0WDT_SYS_RESET");break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : Serial.println ("TG1WDT_SYS_RESET");break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : Serial.println ("RTCWDT_SYS_RESET");break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : Serial.println ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : Serial.println ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : Serial.println ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : Serial.println ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : Serial.println ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : Serial.println ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : Serial.println ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : Serial.println ("NO_MEAN");
  }
}

void verbose_print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1  : Serial.println ("Vbat power on reset");break;
    case 3  : Serial.println ("Software reset digital core");break;
    case 4  : Serial.println ("Legacy watch dog reset digital core");break;
    case 5  : Serial.println ("Deep Sleep reset digital core");break;
    case 6  : Serial.println ("Reset by SLC module, reset digital core");break;
    case 7  : Serial.println ("Timer Group0 Watch dog reset digital core");break;
    case 8  : Serial.println ("Timer Group1 Watch dog reset digital core");break;
    case 9  : Serial.println ("RTC Watch dog Reset digital core");break;
    case 10 : Serial.println ("Instrusion tested to reset CPU");break;
    case 11 : Serial.println ("Time Group reset CPU");break;
    case 12 : Serial.println ("Software reset CPU");break;
    case 13 : Serial.println ("RTC Watch dog Reset CPU");break;
    case 14 : Serial.println ("for APP CPU, reseted by PRO CPU");break;
    case 15 : Serial.println ("Reset when the vdd voltage is not stable");break;
    case 16 : Serial.println ("RTC Watch dog reset digital core and rtc module");break;
    default : Serial.println ("NO_MEAN");
  }
}
