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

// Coordinatorの情報
typedef struct {
  uint32_t h64Add;
  uint32_t l64Add;
  String nodeName;
} XBeeNode;

XBeeNode xbeeCoor = { 0x0013A200, 0x40B77090, "Coordinator" };  

// XBeeをRouterとして起動
EV86XBeeR router = EV86XBeeR();
String senData = "Inoue Shinichi"; // センサー値(仮)
String request = "request";    // コールバック用変数
String startReq = "start"; // 送信命令
String stopReq = "stop"; // 送信停止命令
//boolean transmit = true; // 送受信状態


void setup() {
  Serial.begin(9600);                          // Arduino-PC間の通信
  Serial1.begin(9600);                         // Arduino-XBee間の通信
  Serial1.flush();
  router.begin(Serial1);
  delay(5000); 
  
  
  router.bufFlush();                           // ホストXBeeの内部受信バッファをフラッシュする
  delay(1000);

  router.hsXBeeStatus();                     // ホストXBeeの設定確認用メソッド
  delay(2000);
  
  router.setDstAdd64(xbeeCoor.h64Add, xbeeCoor.l64Add);
  router.rmXBeeStatus();
  Serial.println("Finish checking destination xbee node parameters");
  delay(5000);

  router.broadcastRequest(startReq);                 // リモートXBeeへ要求を出す
}

 void loop() {
  Serial.println("-----------------------------");
  
  // 受信パケットの確認
  Serial.println("[get Packet]");
  router.getPacket();
  delay(300);
 
  // 受信データがコーディネータからのリクエストかチェック
  if (router.checkData(request)) {
    // 受信データがリクエストだった場合
    Serial.print("send :");
    Serial.println(senData);
    router.sendData(senData);
    delay(100);
    Serial.println("[get Packet]");
    router.getPacket();
    
  } // データが来ていない場合
  else {
    Serial.println("No send");
  }
  
  // 受信データの初期化
  router.clearData();
  delay(300);
  
}
