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

String startReq = "startReq";
String startAck = "startAck";
String request = "request";

// Routerの情報
typedef struct {
  uint32_t h64Add;
  uint32_t l64Add;
  String nodeName;
  String startAck;
  String sensorData; // 取得したセンサーデータ
  int timeout;
  boolean transmit;
} XBeeNode;

// ルーター情報の設定
XBeeNode router = { 0x0013A200, 0x40993791, "EDISON", "startAck1", "None", 30, false };
XBeeNode router2 = { 0x0013A200, 0x40707DF7, "MEGA", "startAck2", "None", 30, false };

// プロトタイプ宣言
void connectProcess(XBeeNode& router);
void gettingData(XBeeNode& router);

// コーディネーター用のインスタンスを生成
EV86XBeeC coor = EV86XBeeC();

void setup() {
  Serial.begin(9600);                          // Arduino-PC間の通信
  Serial1.begin(9600);                         // Arduino-XBee間の通信
  Serial1.flush();                             // serial bufferをクリア
  coor.begin(Serial1);                         // XBeeにSerialの情報を渡す
  delay(5000);                                 // XBeeの初期化のために5秒間待機
  
  // ホストXBeeの内部受信バッファをフラッシュする
  coor.bufFlush();
  delay(1000);
  
  // ホストXBeeの設定確認
  coor.hsXBeeStatus();                     
  delay(2000);
  
  // リモートXBeeのアドレス指定と設定情報の取得
  coor.setDstAdd64(router.h64Add, router.l64Add);
  coor.rmXBeeStatus();
  delay(2000);
  
  // リモートXBeeのアドレス指定と設定情報の取得
  coor.setDstAdd64(router2.h64Add, router2.l64Add);
  coor.rmXBeeStatus();
  delay(2000);
  
  //コネクション確立のためのセッション
  connectProcess(router);
  connectProcess(router2);
}

void loop() {  
  Serial.println("[[[[[[[[[[[[[[[ loop start ]]]]]]]]]]]]]]]");
  gettingData(router);
  Serial.println("*******************************************");
  delay(500);  
  gettingData(router2);  
  Serial.println("-------------------------------------------------");
  delay(1000);
}


void connectProcess(XBeeNode& router) {
  // コネクション確立のためのセッション
  /* ------------------------------------------------- */
  // coor →  router
  // coor ← router
  // coor →  router
  
  // ルーターへの接続要求送信
  coor.setDstAdd64(router.h64Add, router.l64Add);
  coor.sendData(startReq);
  Serial.println("send startReq");
  
  // ルーターから接続応答が来ているかチェック
  for (int i = 0; !(coor.getPacket() == ZB_RX_RESPONSE && coor.checkData(router.startAck)); i++) {
    // 受信パケットの確認
    Serial.println("[get Packet]");
   
    // タイムアウトの確認
    Serial.print("timecount : ");
    Serial.println(i);
    
    // timeoutを超過したらルータへの接続要求を再送信する
    if (i > router.timeout) {
      coor.setDstAdd64(router.h64Add, router.l64Add);
      coor.sendData(startReq);
      i = 0;
      Serial.println("send startReq again\n");
    }
    
    // 受信データの初期化
    coor.clearData();
    delay(200);
  }
  
  // 接続応答を送信
  coor.setDstAdd64(router.h64Add, router.l64Add);
  coor.sendData(startAck);
  Serial.println("send startAck\n");
  
  delay(100);
  
  // 受信パケットの確認 
  Serial.println("[get Packet]"); 
  coor.getPacket();
    
  // コネクションの確立
  Serial.print("[[[Connected with Router in ");
  Serial.print(router.nodeName);
  Serial.println("]]]");
  Serial.println("-------------------------------------------------------------");
  router.transmit = true;
  
  // 受信データの初期化
  coor.clearData();
  /* ------------------------------------------------- */
}

void gettingData(XBeeNode& router) {
  // センサーデータ要求
  coor.setDstAdd64(router.h64Add, router.l64Add); // 宛先アドレスの指定
  coor.sendRequest(request);                      // リクエスト送信
  
  // 受信パケットの確認  
  do {
    delay(100);
    // 受信パケットの確認
    Serial.print("[get Packet from ");
    Serial.print(router.nodeName);
    Serial.println("]]]");
  } while (coor.getPacket() >= 0);

  // 受信したセンサーデータ確認
  router.sensorData = coor.getData();
  Serial.print("Sensor Data: "); 
  Serial.println(router.sensorData);
  Serial.println();
  coor.clearData();
}
