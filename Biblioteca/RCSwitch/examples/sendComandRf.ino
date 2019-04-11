/*
O código a seguir - baseado no SendDemo sketch - liga e desliga uma das minhas tomadas de controle remoto a cada 1 segundo. 
Observe que o comprimento de pulso precisa ser definido manualmente porque difere do comprimento de pulso padrão do Protocolo 1. 
Eu criei uma função command()que aceita o número do canal e liga / desliga como argumentos inteiros e procura os endereços de 
dados e comandos correspondentes específicos do meu dispositivo. Para o seu dispositivo, você poderia criar uma função 
semelhante ou apenas enviar os códigos brutos.	

Decimal: 3932352 (24Bit)
Binary: 001111000000000011000000
Tri-State: 011000001000
PulseLength: 232 microseconds
Protocol: 1
Raw data: 7244,232,692,124,792,592,328,596,324,596,328,596,324,140,784,144,788,120,792,136,780,136,788,140,788,128,784,144,
796,124,780,140,784,596,336,588,968,96,36,104,908,132,1412,68,248,64,28,484,56,
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {

 Serial.begin(9600);

 // Transmitter is connected to Arduino Pin #10 
 mySwitch.enableTransmit(10); 

 // Set Protocol (default is 1, will work for most outlets)
 mySwitch.setProtocol(1); 

 // Set pulse length 
 // NB Pulse length must be set AFTER Protocol, 
 // because setProtocol(1) also sets pulse length = 350
 mySwitch.setPulseLength(232);

 // Optional set number of transmission repetitions.
 // Mine seem to work with 2, yours may need more
 mySwitch.setRepeatTransmit(2);
}

// Switch channel 8 on and off every 1 second
void loop() {
 command(8, 1);
 delay(1000);
 command(8, 0);
 delay(1000);
}

void command(int nAddress, int nData) {

 // List of device addresses - may be different for your devices
 char* addressCodes[8] = { "01110000", "00110000", "01010000", "00010000", "01100000", "00100000", "01000000", "00000000" };

 // List of commands - may be different for your devices
 char* dataCodes[2] = { "0000", "1000" };

 // Concatenate the Address and Data codes into a single codeword
 char sendCode[13] = "";
 strcat(sendCode, addressCodes[nAddress-1]);
 strcat(sendCode, dataCodes[nData]);

 // Send the code
 mySwitch.sendTriState(sendCode);
}