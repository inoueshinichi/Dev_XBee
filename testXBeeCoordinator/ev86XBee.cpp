//
//  ev86XBee.cpp
//  
//
//  Created by 井上真一 on 2015/08/04.
//
//

#include "ev86XBee.h"

// コンストラクタ
EV86XBee::EV86XBee() 
: _xbee(), _dstAdd64(), _zbTxRequest(), _zbRxResponse(), _zbTxStatusResponse(), _atResponse(), _rmAtRequest(), _rmAtResponse(), _modemStatus() // コンポジションオブジェクトの初期化リスト
{}


// XBeeとArduino間のシリアル通信の設定
void EV86XBee::begin(Stream &serial) {
  // XBeeオブジェクトにSerial情報を取り次ぐ
  _xbee.begin(serial);
}

void EV86XBee::bufFlush() {
  // xbeeの内部受信バッファをクリアする
  Serial.println("[Buffer Flush]");
  while(getPacket()) {
  };
  Serial.println("-------------------------------------------------");
}

// リモートXBeeへ送信停止要求を出すメソッド
void EV86XBee::stopRequest(String _stop) {
  // ブロードキャストで全員に通達する
  setDstAdd64(0x00000000, 0x00000000);
  sendData(_stop);
}

// 目的地の64-bitAddressをセット
void EV86XBee::setDstAdd64(uint32_t msbAdd, uint32_t lsbAdd) {
  _dstAdd64 = XBeeAddress64(msbAdd, lsbAdd);
}

// データ送信用の基底メソッド
void EV86XBee::sendData(String str) {
  uint8_t reqArray[str.length() + 1]; 
  str.getBytes(reqArray, (str.length() + 1));
  _zbTxRequest = ZBTxRequest(_dstAdd64, reqArray, (uint8_t)sizeof(reqArray)); 
  _xbee.send(_zbTxRequest);
}

// 受信したデータの確認
String EV86XBee::getData() {
  return get_data;
}

// 受信データを初期化する
void EV86XBee::clearData() {
  get_data = "";
}

// ホストXBeeの初期化
void EV86XBee::hsXBeeInit() {
    Serial.println("Init Host XBee...\n");
    delay(1000);
    
    /*
    初期化コマンド
    */
    Serial.println("-------------------------------------------------");
}

// ホストXBeeの設定を確認
void EV86XBee::hsXBeeStatus() {
    Serial.println("Get Host XBee Status...\n");
    delay(1000);
    /*
     Reference getAtCommand(uint8_t* ATcommand, utint8_t* ATparameter)
     if you don't require the parameter, you have to specify "NULL".
     */
    //_xbee.readPacketUntilAvailable(); 
     
    // 設定確認用ATコマンド 
    getATCommand(dbCmd, NULL);      // RSSI (strength of Radio waves)
    getATCommand(idCmd, NULL);      // PAN_ID
    getATCommand(crCmd, NULL);      // Conflicted PAN_IDs
    getATCommand(eeCmd, NULL);      // AES Encryption setting : enable 1, disable 0
    getATCommand(niCmd, NULL);      // Node Identifer
    getATCommand(myCmd, NULL);      // 16bit source address
    getATCommand(dhCmd, NULL);      // High on 16bit address assigned by Coordinator
    getATCommand(dlCmd, NULL);      // Low on 16bit address assigned by Coordinator
    getATCommand(shCmd, NULL);      // High on 64bit unique address
    getATCommand(slCmd, NULL);      // Low on 64bit unique address
    getATCommand(scCmd, NULL);      // Some channels scaned by all Active and Energy Scans
    getATCommand(chCmd, NULL);      // Channed used for this network
    getATCommand(scCmd, NULL);      // Scan Duration
    getATCommand(njCmd, NULL);      // Node Join Time
    getATCommand(ncCmd, NULL);      // number of remaining children
    getATCommand(aiCmd, NULL);      // Association Indication
    getATCommand(hvCmd, NULL);      // XBee Hardware Version
    getATCommand(vrCmd, NULL);      // XBee Firmware Version
    
    Serial.println("-------------------------------------------------");
}

// リモートXBeeの設定の初期化
void EV86XBee::rmXBeeInit() {
  Serial.println("Init Remote XBee...");
  Serial.print("Address64[0x");
    Serial.print(_dstAdd64.getMsb(), HEX);
    Serial.print(" 0x");
    Serial.print(_dstAdd64.getLsb(), HEX);
    Serial.print("]\n");
    Serial.println();
    delay(1000);
     
    /*
    初期化コマンド
    */
    Serial.println("-------------------------------------------------");
}

// リモートXBeeの設定を確認
void EV86XBee::rmXBeeStatus() {
    Serial.println("Get Remote XBee...");
    Serial.print("Address64[0x");
    Serial.print(_dstAdd64.getMsb(), HEX);
    Serial.print(" 0x");
    Serial.print(_dstAdd64.getLsb(), HEX);
    Serial.print("]\n");
    Serial.println();
    delay(1000);
    
    //確認リモートATコマンド
    getRemoteATCommand(dbCmd, NULL);    // RSSI (strength of Radio waves)
    getRemoteATCommand(idCmd, NULL);    // PAN_ID
    getRemoteATCommand(crCmd, NULL);    // Conflicted PAN_IDs
    getRemoteATCommand(eeCmd, NULL);    // AES Encryption setting : enable 1, disable 0
    getRemoteATCommand(niCmd, NULL);    // Node Identifer
    getRemoteATCommand(myCmd, NULL);    // 16bit source address
    getRemoteATCommand(dhCmd, NULL);    // High on 16bit address assigned by Coordinator
    getRemoteATCommand(dlCmd, NULL);    // Low on 16bit address assigned by Coordinator
    getRemoteATCommand(shCmd, NULL);    // High on 64bit unique address
    getRemoteATCommand(slCmd, NULL);    // Low on 64bit unique address
    getRemoteATCommand(scCmd, NULL);    // Some channels scaned by all Active and Energy Scans
    getRemoteATCommand(chCmd, NULL);    // Channed used for this network
    getRemoteATCommand(scCmd, NULL);    // Scan Duration
    getRemoteATCommand(njCmd, NULL);    // Node Join Time  
    getRemoteATCommand(ncCmd, NULL);    // Number of remaining children
    getRemoteATCommand(aiCmd, NULL);    // Association Indication
    getRemoteATCommand(hvCmd, NULL);    // XBee Hardware Version
    getRemoteATCommand(vrCmd, NULL);    // XBee Firmware Version
    
    Serial.println("-------------------------------------------------");
}
         
// ATコマンド用のAPIフレーム送受信プログラム
void EV86XBee::getATCommand(uint8_t* sendCmd, uint8_t* cmdValue) {
    // XBeeにリクエストを送る
    
    // コマンドにパラメータ値が必要な場合
    if (cmdValue != NULL) {
        _atRequest = AtCommandRequest(sendCmd, cmdValue, sizeof(cmdValue));
    }
    // コマンドだけでよい場合
    else {
        _atRequest = AtCommandRequest(sendCmd);
    }
    
    // XBeeにATコマンド要求を出す
    _xbee.send(_atRequest);
    // レスポンスパケットまで待機(不可変でセッティングしてる)
    delay(100);
    
    //　返答パケットを取得
    getPacket();
}

// リモートATコマンド用のAPIフレーム送受信プログラム
void EV86XBee::getRemoteATCommand(uint8_t* sendCmd, uint8_t* cmdValue) {
    // リモートで他のXBeeにリクエストを送る
    
    // コマンドにパラメータ値が必要な場合
    if (cmdValue != NULL) {
        _rmAtRequest = RemoteAtCommandRequest(_dstAdd64, sendCmd, cmdValue, sizeof(cmdValue));
    }
    // コマンドだけで良い場合
    else {
        _rmAtRequest = RemoteAtCommandRequest(_dstAdd64, sendCmd);
    }
    
    // リモートで指定したXBeeにATコマンド要求を出す
    _xbee.send(_rmAtRequest);
    // レスポンスパケットまで待機(不可変でセッティングしてる) 無線の場合遅延が大きので、もしかしたら、足りないかも
    delay(100);
    
    // 返答パケットを取得
    getPacket();
}

// XBeeからの受信Packetを解析関数
boolean EV86XBee::getPacket() { // デフォルト引数はプロトタイプ宣言部のみに記述する　(get_data = "")
    boolean flag = true;

    // APIフレームを受信
    _xbee.readPacket();
    
    // APIフレーム内にデータがあるか確認
    if (_xbee.getResponse().isAvailable()) {
        // APIフレームの先頭1バイト(API Id)を読み込んで、以降のAPIフレーム情報が何かを確認　ex 通常のデータ, ATコマンドレスポンス, リモートATコマンドレスポンスetc..
        switch (_xbee.getResponse().getApiId()) {
                
            //////////////////////////////////////通常のデータパケット///////////////////////////////////////
            case ZB_RX_RESPONSE : // (Zigbee Receive Packet)
                Serial.println("Frame ID : [ZB_RX_RESPONSE]");
                _xbee.getResponse().getZBRxResponse(_zbRxResponse);
                
                // 宛名アドレスの取得
                _srcAdd64 = _zbRxResponse.getRemoteAddress64(); 
               
                // 受信データの取得
                for (int i = 0; i < 16; i++) {
                    // APIフレーム内のデータがゼロまたはAPIフレーム長以上のデータを要求しようとした場合、break
                    if (_zbRxResponse.getData()[i] == 0x00 || i >= _zbRxResponse.getDataLength())
                        break;
                   
                    //Serial.println((char)_zbRxResponse.getData()[i]);
                    get_data.concat((char)_zbRxResponse.getData()[i]); // 受信した文字配列を連結してString型に格納
                }   
                  
                // 送られてきたデータを確認
                Serial.print("Get Data: ");
                for (int i = 0; i < get_data.length(); i++) {         
                  Serial.print(get_data.charAt(i));
                }
                Serial.print("\n");
                Serial.print("length:");
                Serial.println(get_data.length());
                Serial.println();
                break;
            
//            //////////////////////////////////////ClusterIDを含んだデータパケット///////////////////////////////////////
//            case ZB_EXPLICIT_RX_RESPONSE :
//                Serial.println("Frame ID : [ZB_EXPLICIT_RX_RESPONSE]");
//                Serial.println("Zb_explicit_rx_response has not implemented yet.");
//                break;
//            
//            //////////////////////////////////////IO_Sample用のデータパケット///////////////////////////////////////
//            case ZB_IO_SAMPLE_RESPONSE : 
//                Serial.println("Frame ID : [ZB_IO_SAMPLE_RESPONSE]");
//                Serial.println("Zb_io_sample_response has not implemented yet.");
//                break;
                
            //////////////////////////////////////送信パケットが無事に送信されたかどうかのチェックレスポンス///////////////////////////////////////
            case ZB_TX_STATUS_RESPONSE :
                _xbee.getResponse().getZBTxStatusResponse(_zbTxStatusResponse);
                Serial.println("Frame ID : [ZB_TX_STATUS_RESPONSE]");
                if (_zbTxStatusResponse.getRemoteAddress() == 0xFFFD) {
                  Serial.println("Destination Address is unknown");
                } else {
                  Serial.print("dstAdd16 0x");
                  Serial.println(_zbTxStatusResponse.getRemoteAddress(), HEX);
                }
                
                switch (_zbTxStatusResponse.getDeliveryStatus()) {
                  case SUCCESS : 
                      Serial.println("Success!");
                      break;
                  case MAC_ACK_FAILURE :
                      Serial.println("Mac ACK Failure");
                      break;
                  case CCA_FAILURE :
                      Serial.println("CCA Failure");
                      break;
                  case INVALID_DESTINATION_ENDPOINT :
                      Serial.println("Invalid Destination Endpoint");
                      break;
                  case NETWORK_ACK_FAILURE :
                      Serial.println("Network ACK Failure");
                      break;
                  case NOT_JOINED_TO_NETWORK :
                      Serial.println("Not Joined To Network");
                      break;
                  case SELF_ADDRESSED : 
                      Serial.println("Self-Addressed");
                      break;
                  case ADDRESS_NOT_FOUND :
                      Serial.println("Address Not Found");
                      break;
                  case ROUTE_NOT_FOUND :
                      Serial.println("Route Not Found");
                      break;
                  case BROADCAST_SOURCE_FAILED_TO_HEAR_A_NEIGHBOR_RELAY_THE_MESSAGE :
                      Serial.println("Broadcast source failed to hear a neighbor relay the message");
                      break;
                  case INVALID_BINDING_TABLE_INDEX :
                      Serial.println("Invalid Binding Table Index");
                      break;
                  case RESOURCE_ERROR_LACK_OF_FREE_BUFFERS_TIMERS_AND_SO_FORTH :
                      Serial.println("Resource error lack of free buffers timers and so forth");
                      break;
                  case ATTEMPTED_BROADCAST_WITH_APS_TRANSMISSION :
                      Serial.println("Attempted Broadcast With Transmission");
                      break;
                  case ATTEMPTED_UNICAST_WITH_APS_TRANSMISSION_BUT_EE :
                      Serial.println("Attempted unicast with transmission but EE=0");
                      break;
                  case RESOURCE_EEROR_LACK_OF_FREE_BUFFERS_TIMERS_AND_SO_FORTH :
                      Serial.println("Resource error lack of free buffers timers and so forth");
                      break;
                  case PAYLOAD_TOO_LARGE :
                      Serial.println("Payload too large");
                      break;
                      
                  default :
                      Serial.println("Error");
                      break;
                }
//                Serial.print("TxRetryCount    "); Serial.println(_zbTxStatusResponse.getTxRetryCount());
//                Serial.print("DeliveryStatus  "); Serial.println(_zbTxStatusResponse.getDeliveryStatus());
//                Serial.print("DiscoveryStatus "); Serial.println(_zbTxStatusResponse.getDiscoveryStatus());
                Serial.println();
                break;
                
            /////////////////////////////////ATコマンドレスポンスパケット//////////////////////////////////////
            case AT_COMMAND_RESPONSE :
                Serial.println("Frame ID : [AT_COMMAND_RESPONSE]");
                _xbee.getResponse().getAtCommandResponse(_atResponse);
                
                // ATコマンドレスポンス(これもAPIフレーム形式)データが壊れていないかチェック
                if (_atResponse.isOk()) {
                    // ATコマンドを表示
                    Serial.print("Command [ ");
                    Serial.print((char)_atResponse.getCommand()[0]);
                    Serial.print((char)_atResponse.getCommand()[1]);
                    Serial.println(" ] success!");
                    
                    // ATコマンドレスポンスのデータがある場合
                    if (_atResponse.getValueLength() > 0) {
                        Serial.print("Command value length is ");
                        Serial.println(_atResponse.getValueLength(), DEC);
                        Serial.print("Command value: ");
                        Serial.print("[HEX] ");
                        
                        // 受信データをHEXで表示(ATコマンドレスポンスデータはHEX形式で送られてくる.つまりビット)
                        for (int i = 0; i < _atResponse.getValueLength(); i++) {
                            Serial.print(_atResponse.getValue()[i], HEX);
                        }
                        Serial.println("\n");
                        
                    }
                    // ATコマンドレスポンスが不適切な場合
                    else {
                        Serial.print("Command return error code: ");
                        Serial.print(_atResponse.getStatus(), HEX);
                        switch(_atResponse.getStatus()) {
                                // そもそもレスポンスデータが外部要因によって壊れている
                            case AT_ERROR :
                                Serial.println(" ERROR");
                                break;
                                // 送ったATコマンドが不適切
                            case AT_INVALID_COMMAND :
                                Serial.println(" INVALID_COMMAND");
                                break;
                                // 送ったATコマンドパラメータが不適切
                            case AT_INVALID_PARAMETER :
                                Serial.println(" AT_INVALID_PARAMETER");
                                break;
                                // ATリクエストの送信に失敗してた
                            case AT_NO_RESPONSE :
                                Serial.println(" AT_Command Transmission was failed");
                                break;
                        }
                    }
                }
                break;
                
                
            /////////////////////////////////リモートATコマンドレスポンスパケット//////////////////////////////////////
            case REMOTE_AT_COMMAND_RESPONSE :
                Serial.println("Frame ID : [REMOTE_AT_COMMAND_RESPONSE]");
                _xbee.getResponse().getRemoteAtCommandResponse(_rmAtResponse);
                
                // リモートATコマンドレスポンス(これもAPIフレーム形式)データが壊れていないかチェック
                if (_rmAtResponse.isOk()) {
                    // リモートATコマンドを表示
                    Serial.print("Command [ ");
                    Serial.print((char)_rmAtResponse.getCommand()[0]);
                    Serial.print((char)_rmAtResponse.getCommand()[1]);
                    Serial.println(" ] success!");
                    
                    // ATコマンドレスポンスのデータがある場合
                    if (_rmAtResponse.getValueLength() > 0) {
                        Serial.print("Command value length is ");
                        Serial.println(_rmAtResponse.getValueLength(), DEC);
                        Serial.print("Command value: ");
                        Serial.print("[HEX] ");
                        
                        // 受信データをHEXで表示(ATコマンドレスポンスデータはHEX形式で送られてくる.つまりビット)
                        for (int i = 0; i < _rmAtResponse.getValueLength(); i++) {
                            Serial.print(_rmAtResponse.getValue()[i], HEX);
                        }
                        Serial.println("\n");
                        
                    }
                    // ATコマンドレスポンスが不適切な場合
                    else {
                        Serial.print("Command return error code: ");
                        Serial.print(_rmAtResponse.getStatus(), HEX);
                        switch(_rmAtResponse.getStatus()) {
                                // そもそもレスポンスデータが外部要因によって壊れている
                            case AT_ERROR :
                                Serial.println(" ERROR");
                                break;
                                // 送ったATコマンドが不適切
                            case AT_INVALID_COMMAND :
                                Serial.println(" INVALID_COMMAND");
                                break;
                                // 送ったATコマンドパラメータが不適切  
                            case AT_INVALID_PARAMETER : 
                                Serial.println(" AT_INVALID_PARAMETER"); 
                                break; 
                                // ATリクエストの送信に失敗してた
                            case AT_NO_RESPONSE :
                                Serial.println(" AT_Command Transmission was failed");
                                break;
                        } 
                    }
                }
                break;
                
            case MODEM_STATUS_RESPONSE :
                Serial.println("Frame ID : [MODEM_STATUS_RESPONSE]");
                switch (_modemStatus.getStatus()) {
                  case HARDWARE_RESET :
                      Serial.println("HARDWARE_RESET");
                      break;
                  case WATCHDOG_TIMER_RESET :
                      Serial.println("WATCHDOG_TIMER_RESET");
                      break;
                  case ASSOCIATED :
                      Serial.println("ASSOCIATED");
                      break;
                  case DISASSOCIATED :
                      Serial.println("DISASSOCIATED");
                      break;
                  case SYNCHRONIZATION_LOST :
                      Serial.println("SYNCHRONIZATION_LOST");
                      break;
                  case COORDINATOR_REALIGNMENT :
                      Serial.println("COORDINATOR_REALIGNMENT");
                      break;
                  case COORDINATOR_STARTED :
                      Serial.println("COORDINATOR_STARTED");
                      break;
                  case NETWORK_SECURITY_KEY_WAS_UPDATED :
                      Serial.println("NETWORK_SECURITY_KEY_WAS_UPDATED");
                      break;
                  case VALTAGE_SUPPLY_LIMIT_EXCEEDED :
                      Serial.println("VALTAGE_SUPPLY_LIMIT_EXCEEDED");
                      break;
                  case MODEM_CONFIGURATION_CHANGED_WHILE_JOIN_IN_PROGRESS :
                      Serial.println("MODEM_CONFIGURATION_CHANGED_WHILE_JOIN_IN_PROGRESS");
                      break;
                  case STACK_ERROR :
                      Serial.println("STACK_ERROR");
                      break;
                  default:
                      Serial.println("Other ERROR");
                      break;
                }
                break;
//                
//            case XBEE_SENSOR_READ_INDICATOR :
//                Serial.println("Frame ID : [XBEE_SENSOR_READ_INDICATOR]");
//                Serial.println("Xbee_sensor_read_indicator has not implemented yet.");
//                break;
//                
//            case ZB_IO_NODE_IDENTIFIER_RESPONSE :
//                Serial.println("Frame ID : [ZB_IO_IDENTIFIER_RESPONSE]");
//                Serial.println("Zb_io_node_indentifier_response has not implemented yet.");
//                break;
//            
//            case OVER_THE_AIR_FIRMWARE_UPDATE_STATUS :
//                Serial.println("Frame ID : [OVER_THE_AIR_FIRMWARE_UPDATE_STATUS]");
//                Serial.println("Over-the-air-firmware-update-status has not implemented yet.");
//                break;
//            
//            case ROUTE_RECORD_INDICATOR :
//                Serial.println("ROUTE_RECORD_INDICATOR");
//                Serial.println("Route_recode_indicator  has not implemented yet.");
//                break;
//            
//            case ZB_MANY_TO_ONE_ROUTE_REQUEST_INDICATOR :
//                Serial.println("ZB_MANY_TO_ONE_ROUTE_REQUEST_INDICATOR");
//                Serial.println("Many-to-one-route-request-indicator  has not implemented yet.");
//                break;
                
            /////////////////////その他のAPI Idを受け取った場合、ここはIdの種類に応じて各自拡張できる//////////////////////  
            default: 
                Serial.print("Other API Data Frame ");
                Serial.println(_xbee.getResponse().getApiId(), HEX);
                break;        
        }
    }
    // パケットが受信できない
    else {
        // パケットのデータ形式が壊れている 
        if (_xbee.getResponse().isError()) {
            uint8_t error = _xbee.getResponse().getErrorCode();
            Serial.print("Error reading packet. Error code: ");
            Serial.println(error);
            switch (error) {
              case CHECKSUM_FAILURE :
                Serial.println("CHECKSUM_FAILURE");
                break;
              case PACKET_EXCEEDS_BYTE_ARRAY_LENGTH :
                Serial.println("PACKET_EXCEEDS_BYTE_ARRAY_LENGTH");
                break;
              case UNEXPECTED_START_BYTE :
                Serial.println("UNEXPECTED_START_BYTE");
                break;
              default :
                Serial.println("UNKNOWN ERROR");
                break;
            }
        }
        // そもそもパケットが受信できない つまり、ハードウェアトラブルかPANに他のXBeeデバイスがいない
        else { 
            Serial.println("No response from radio."); 
            flag = false; // 内部受信バッファに何もなくなったら状態を変える
        } 
    }
  return flag; // XBee内部の受信バッファ内のデータの状態を返す
}

boolean EV86XBee::checkData(String request) {
  if (get_data.equals(request)) {
    Serial.println("SUCCESS!!");
    return true;
  } else {
    Serial.println("ERROR");
    return false;
  }
}


/*                 Coordinator                    */



// コンストラクタ
EV86XBeeC::EV86XBeeC() 
: EV86XBee() // 親クラスのコンストラクタ呼び出し
{}

// センサーデータ要求用メソッド
void EV86XBeeC::sendRequest(String request) {  
  Serial.println("[Send sensor request from Coordinator]\n");
  sendData(request);
  
}



/*                 Router                    */



// コンストラクタ
EV86XBeeR::EV86XBeeR() 
: EV86XBee() // 親クラスのコンストラクタ呼び出し
{}



//void EV86XBeeR::sendSensorData(String send_buf) {
//   
//  //  // 送信データ作成            
//  send_buf.concat((char)send_count);
//  send_count++;
//  if (send_count >= 10) {
//    send_count = 0;
//  }   
//  uint8_t send_data[send_buf.length()]; // 送信データ用配列   
//  
//  // 送信の準備
//  for (int i = 0; i < send_buf.length(); i++) {
//    send_data[i] = send_buf.charAt(i);
//  }
//  
//  //送信データの確認
//  Serial.print("Send Data:");
//  for (int i = 0; i < send_buf.length(); i++){
//    Serial.print((char)send_data[i]);
//  }
//  Serial.print("\n");
//   
//  //データ送信
//  _srcAdd64 = _zbRxResponse.getRemoteAddress64();                        // 宛名アドレスの取得
//  _dstAdd64 = _srcAdd64;                                                 // 宛先アドレスに受信データの宛名アドレスを指定する
//  _zbTxRequest = ZBTxRequest(_dstAdd64, send_data, sizeof(send_data)); 　//送信API作成
//  _xbee.send(_zbTxRequest);  //送信
//}
