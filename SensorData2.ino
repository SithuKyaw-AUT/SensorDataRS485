/**
 * @file SensorData
 * @author Sithu
 * @brief This sketch reads data from sensor and send the data to Lora Gateway.
 * @version 0.1
 * @date 2022-09-05
 */


#include<ArduinoRS485.h>
 
#define SensorData_PERIOD (20000)
/*************************************

   LoRaWAN band setting:
     RAK_REGION_EU433
     RAK_REGION_CN470
     RAK_REGION_RU864
     RAK_REGION_IN865
     RAK_REGION_EU868
     RAK_REGION_US915
     RAK_REGION_AU915
     RAK_REGION_KR920
     RAK_REGION_AS923

 *************************************/
#define SensorData_BAND     (RAK_REGION_AS923)
// update the DeviceEUI 
#define SensorData_DEVEUI   {0xAC, 0x1F, 0x09, 0xFF, 0xFE, 0x06, 0xD3, 0x27}
// update the APPEUI
// #define SensorData_APPEUI   {0x0E, 0x0D, 0x0D, 0x01, 0x0E, 0x01, 0x02, 0x0E}
#define SensorData_APPEUI   {0xBE, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x02, 0xC5}
// update APPKEY
// #define SensorData_APPKEY   {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3E}
#define SensorData_APPKEY   {0xBC, 0x83, 0x05, 0xFF, 0x2C, 0x0B, 0x85, 0x46, 0x2D, 0xA0, 0x9E, 0xC6, 0x6B, 0x36, 0x83, 0x17}

/** Packet buffer for sending */
uint8_t collected_data[64] = { 0 };

void recvCallback(SERVICE_LORA_RECEIVE_T * data)
{
  if (data->BufferSize > 0) {
    Serial.println("Something received!");
    for (int i = 0; i < data->BufferSize; i++) {
      Serial.printf("%x", data->Buffer[i]);
    }
    Serial.print("\r\n");
  }
}

void joinCallback(int32_t status)
{
  Serial.printf("Join status: %d\r\n", status);
}

void sendCallback(int32_t status)
{
  if (status == 0) {
    Serial.println("Successfully sent");
  } else {
    Serial.println("Sending failed");
  }
}

void setup() {
  Serial.begin(115200, RAK_AT_MODE);
  // begin RS485
  RS485.begin(9600);
  
  Serial.println("RAKwireless LoRaWan OTAA Example");
  Serial.println("------------------------------------------------------");

  // OTAA Device EUI MSB first
  uint8_t node_device_eui[8] = SensorData_DEVEUI;
  // OTAA Application EUI MSB first
  uint8_t node_app_eui[8] = SensorData_APPEUI;
  // OTAA Application Key MSB first
  uint8_t node_app_key[16] = SensorData_APPKEY;

  if (!api.lorawan.appeui.set(node_app_eui, 8)) {
    Serial.printf("LoRaWan OTAA - set application EUI is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.appkey.set(node_app_key, 16)) {
    Serial.printf("LoRaWan OTAA - set application key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deui.set(node_device_eui, 8)) {
    Serial.printf("LoRaWan OTAA - set device EUI is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.band.set(SensorData_BAND)) {
    Serial.printf("LoRaWan OTAA - set band is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
    Serial.printf("LoRaWan OTAA - set device class is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.njm.set(RAK_LORA_OTAA))  // Set the network join mode to OTAA
  {
    Serial.printf
  ("LoRaWan OTAA - set network join mode is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.join())  // Join to Gateway
  {
    Serial.printf("LoRaWan OTAA - join fail! \r\n");
    return;
  }

  /** Wait for Join success */
  while (api.lorawan.njs.get() == 0) {
    Serial.print("Wait for LoRaWAN join...");
    api.lorawan.join();
    delay(10000);
  }

  if (!api.lorawan.adr.set(true)) {
    Serial.printf
  ("LoRaWan OTAA - set adaptive data rate is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.rety.set(1)) {
    Serial.printf("LoRaWan OTAA - set retry times is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.cfm.set(1)) {
    Serial.printf("LoRaWan OTAA - set confirm mode is incorrect! \r\n");
    return;
  }

  /** Check LoRaWan Status*/
  Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF");  // Check Duty Cycle status
  Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");  // Check Confirm status
  uint8_t assigned_dev_addr[4] = { 0 };
  api.lorawan.daddr.get(assigned_dev_addr, 4);
  Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);  // Check Device Address
  Serial.printf("Uplink period is %ums\r\n", SensorData_PERIOD);
  Serial.println("");
  api.lorawan.registerRecvCallback(recvCallback);
  api.lorawan.registerJoinCallback(joinCallback);
  api.lorawan.registerSendCallback(sendCallback);
}

void uplink_routine()
{
  // enable reception of RS485
  RS485.receive();
  
  /** Payload of Uplink */
  uint8_t data_len = 0;

  //to test data sending to lorawan
  //collected_data[data_len++] = (uint8_t) 't';     
  //collected_data[data_len++] = (uint8_t) 'e';     
  //collected_data[data_len++] = (uint8_t) 's';     
  //collected_data[data_len++] = (uint8_t) 't';     

  // read the data and save it in the collected_data array
  for(data_len = 0; data_len < RS485.available(); data_len++){
    collected_data[data_len] = RS485.read() & 0x7F;
    if(collected_data[data_len] == 13){
      collected_data[data_len] = '\0';
      break;
    }
  }
 
  Serial.println(" \n\n\n");
  Serial.println("Data Packet:");
  for (int i = 0; i < data_len; i++) {
    Serial.printf("0x%02X ", collected_data[i]);
  }
  Serial.println("");

  /** Send the data package */
  if (api.lorawan.send(data_len, (uint8_t *) & collected_data, 2, true, 1)) {
    Serial.println("Sending is requested");
  } else {
    Serial.println("Sending failed");
  }

}

void loop() {
  RAK_LORA_chan_rssi chan_arssi;

  // being Transmission of RS485
  RS485.beginTransmission();
  
  /* RS485 Power On */
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
  /* RS485 Power On */
  //RS485.sendBreakMicroseconds(12000);
  //delayMicroseconds(9000);

  // how to get the data from sensor
  char send_buffer[4];
  RS485.write(send_buffer);
  RS485.flush();

  delay(380);
  
  uplink_routine();

  // end Transmission of RS485
  RS485.endTransmission();

  /* RS485 Power Off */
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, LOW);
  delay(300);
  /* RS485 Power Off */
  
  Serial.print("Get RSSI =");
  Serial.println(api.lorawan.rssi.get());

  Serial.print("Get Signal Noise Ratio =");
  Serial.println(api.lorawan.snr.get());

  Serial.print("Get LoRaWAN protocol version =");
  Serial.println(api.lorawan.ver.get());

  Serial.print("Get All open channel RSSI =");
  while (api.lorawan.arssi(&chan_arssi) == true) {
    if (chan_arssi.mask != 0) {
      Serial.printf("channel : %d, rssi : %d\r\n", chan_arssi.chan,
        chan_arssi.rssi);
    }
  }

  Serial.print("\r\n");
  delay(SensorData_PERIOD);
}
