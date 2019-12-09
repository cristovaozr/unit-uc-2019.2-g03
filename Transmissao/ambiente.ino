void ambiente_Valores() {
  ///ler o valor do LDR
  pack_envio.ldrValor = analogRead(ldrPin); //O valor lido será entre 0 e 1023
   pack_envio.umidadeValor = analogRead(umidade); //O valor lido será entre 0 e 1023
   temp_umid();
    if (millis() - timer >= 2000)
  {
   radio.stopListening();          //para de ler informaçoes do outro pipe
  
    checagem(radio.write(&pack_envio, sizeof(pack_envio)));
    
    alterado_1 = false;

    radio.flush_tx();

    timer = millis(); // Atualiza a referência
    Serial.println("");
  }
}

void temp_umid()
{

  DHT.read11(pinoDHT11); //LÊ AS INFORMAÇÕES DO SENSOR
  pack_envio.umid = DHT.humidity;
  pack_envio.temp = DHT.temperature;


}
