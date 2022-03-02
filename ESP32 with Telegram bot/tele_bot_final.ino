#include <WiFi.h>//libraries for setting up wifi
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>//for telegram
#include <ArduinoJson.h>
//credentials
const char* ssid = "vivo 1951";
const char* password = "12345678";
//we need these tokens so that we can interact with the bot
#define BOTtoken "2136800120:AAHN8RK8JJP-L-Pj5lBWoIo6zbINPGP3pBo"  
#define CHAT_ID "933504755" 

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int bot_delay = 1000;
unsigned long lastTimeBotRan;
const int ledPin = 2;
bool ledState = LOW;

//Handling new messages 
void handleNewMessages(int numNewMessages) {
  Serial.println("Handling New Message");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
   
    String chat_id = String(bot.messages[i].chat_id);
    //verifying the chat id
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String user_text = bot.messages[i].text;
    Serial.println(user_text);

    String your_name = bot.messages[i].from_name;
    //start command
    if (user_text == "/start") {
      String welcome = "Welcome, " + your_name + ".\n";
      welcome += "Use the following commands to control the ESP32.\n\n";
      welcome += "Send /led_on to turn GPIO2 ON \n";
      welcome += "Send /led_off to turn GPIO2 OFF \n";
      welcome += "Send /get_status to request current GPIO2 stateus \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    //ledon command 
    if (user_text == "/led_on") {
      bot.sendMessage(chat_id, "LED is now ON", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    //led off command
    if (user_text == "/led_off") {
      bot.sendMessage(chat_id, "LED is now OFF", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    //and last one get status command
    if (user_text == "/get_status") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LED is ON.", "");
      }
      else{
        bot.sendMessage(chat_id, "LED is OFF.", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + bot_delay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("Got Response!");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
