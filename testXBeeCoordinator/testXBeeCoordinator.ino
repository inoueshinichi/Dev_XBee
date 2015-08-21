/*
*XBee APIフレーム用送受信プログラム
*随時更新していきます。
*現在できている項目
*  1: 宛先Addressの設定
*  2: 通常データの送受信
*  3: ATコマンドによるホストXBeeへの要求と応答の受け取り
*
*/

// ヘッダーのインクルード
#include "ev86XBee.h"

// Routerの情報
typedef struct {
  uint32_t h64Add;
  uint32_t l64Add;
  String nodeName;
} XBeeNode;

// ルーター情報の設定
XBeeNode xbeeRouter = { 0x0013A200, 0x40993791, "Router"};

EV86XBeeC coor = EV86XBeeC();
String sensorData = ""; // センサー値(仮)
String request = "request";
String startReq = "start";
String stopReq = "stop";
boolean transmit = true;


void setup() {
  Serial.begin(9600);                          // Arduino-PC間の通信
  Serial1.begin(9600);                         // Arduino-XBee間の通信
  Serial1.flush();
  coor.begin(Serial1);                     // XBeeにSerialの情報を渡す
  delay(5000);                                 // XBeeの初期化のために5秒間待機
  
  // ホストXBeeの内部受信バッファをフラッシュする
  coor.bufFlush();
  delay(1000);
  
  coor.hsXBeeStatus();                     // ホストXBeeの設定確認
  delay(2000);
  
  // リモートXBeeのアドレス指定と設定情報の取得
  coor.setDstAdd64(xbeeRouter.h64Add, xbeeRouter.l64Add);
  coor.rmXBeeStatus();
  Serial.println("Finish checking destination xbee node parameters");
  delay(5000);
}

void loop() {
  if (transmit) {
    // 受信パケットの確認
    Serial.println("[get packet]");
    coor.getPacket();
    // 受信したセンサーデータ確認
    sensorData = coor.getData();
    Serial.print("Sensor Data: "); 
    Serial.println(sensorData);
    
    // 送信開始要求だった場合
    if (coor.checkData(startReq)) {
      transmit = false;
    }
    
    // 受信データの初期化
    coor.clearData();
    
  } else {  
    // センサーデータ要求
    coor.setDstAdd64(xbeeRouter.h64Add, xbeeRouter.l64Add); // 宛先アドレスの指定
    coor.sendRequest(request); // リクエスト送信
    delay(100);
    Serial.println("[get packet]");
    coor.getPacket();
    delay(100);
 
    // 受信パケットの確認
    Serial.println("[get packet]");
    coor.getPacket();
  
    // 受信したセンサーデータ確認
    sensorData = coor.getData();
    Serial.print("Sensor Data: "); 
    Serial.println(sensorData);
    Serial.println();
    coor.clearData();
    Serial.println("-------------------------------------------------");
  }
  delay(1000);  
}
