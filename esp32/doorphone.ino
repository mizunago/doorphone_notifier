#include <WiFiMulti.h>
#include <HTTPClient.h>

// 管理者権限を持っているアカウントでテキストチャットを右クリック->チャンネルの編集->連携サービス->ウェブフックを作成してURLを貼る
const char URL[] = "https://discordapp.com/api/webhooks/XXXXXXXXX/YYYYYYYY";
WiFiMulti wifiMulti;
int input_val = 0;
#define D_PIN 32 // デジタル入力ピン
#define LED_BUILTIN 2  // ビルトインLEDピン

void setup() {
  Serial.begin(115200); // このシリアル通信はモニター用
  delay(10);

  pinMode(D_PIN, INPUT);

  Serial.print("wifi connecting...\n");
  // Wi-Fi 接続 https://www.sglabs.jp/esp-wroom-02-wi-fi/
  wifiMulti.addAP("SSID-NAME-1", "password1");
  wifiMulti.addAP("SSID-NAME-2", "password2");
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("wifi connected\n");
}

void loop() {
  input_val = digitalRead(D_PIN);
  if (input_val == HIGH) {
    blynkBuiltInLed();
    sendMessage();
    // インターホンが押されたあとは、誰も応答しなくても、しばらく電源が入っている
    // 再送を防ぐため、60秒待つ
    delay(60000);
  }
  // 0.5 秒おきにチェックする
  delay(500);
}

void sendMessage() {
  String httpRequestData = "{\"content\": \"ドアホンの電源が入りました\"}";
  HTTPClient http;
  http.begin(URL);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(httpRequestData);

  Serial.printf("Response: %d", httpCode);
  Serial.println();
  if (httpCode == HTTP_CODE_OK) {
    String body = http.getString();
    Serial.print("Response Body: ");
    Serial.println(body);
  }
  http.end();

  Serial.println("closing connection");
}

void blynkBuiltInLed() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
}
