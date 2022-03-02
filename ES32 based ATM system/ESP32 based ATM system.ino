//included all the necessary libraries
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "ThingSpeak.h"
#include "HTTPClient.h"

// wifi credentials
const char* ssid = "vivo 1951";
const char* password = "12345678";

// bot token for interacting with the bot we made for our ATM system
#define BOTtoken "2104897644:AAFFaA1nHqm12rooXlc8_sEqEuZbrd229g8"
//chat id -- for making sure that we only are interactig with our telebot, no one else
#define CHAT_ID "933504755"  //replace with your telegram user ID

//// channel no and api keys for publishing data on thingspeak
//unsigned long myChannelNumber = 1591008;
//const char * myWriteAPIKey = "YJ03A6JP1BI24U9Z";

//defining thingspeak urls and apiKeys to write over the values
const char* serverName = "https://api.thingspeak.com/update";
String apiKey1 = "5X3AKK3QUULTMXY8"; //APIKey to display balance amout
String apiKey2 = "L1B1FJCSJ83AAOIH"; // API Key to diaplay denominations of Rs.2000/-
String apiKey3 = "G7TE9M14662B9BIR"; // API Key to display denominations of Rs.1000/-
String apiKey4 = "EQA99804UZFIHQ9X"; // API Key to display denominations of Rs.500/-

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int bot_delay = 1000;
unsigned long lastTimeBotRan;
const int touch0 = 4;
const int touch1 = -1;
const int touch2 = 2;
const int touch3 = 15;
const int touch4 = 13;
const int touch5 = 12;
const int touch6 = 14;
const int touch7 = 27;
const int touch8 = 33;
const int touch9 = 32;

int v;
//getdigit function to make touch pins of esp to act like a button by setting some threshhold
//here threshold is 30
int getdigit() {
  v = touchRead(touch0);
  if (v < 30)
    return 0;

  v = touchRead(touch2);
  if (v < -1)
    return 2;

  v = touchRead(touch3);
  if (v < 30)
    return 3;

  v = touchRead(touch4);
  if (v < 30)
    return 4;

  v = touchRead(touch5);
  if (v < 30)
    return 5;

  v = touchRead(touch6);
  if (v < 30)
    return 6;

  v = touchRead(touch7);
  if (v < 30)
    return 7;

  v = touchRead(touch8);
  if (v < 30)
    return 8;

  v = touchRead(touch9);
  if (v < 30)
    return 9;
  return -1;
}

//function for verifying otp by comparing individual digits of otp generated and
//otp we entered, if they match, function will return true ,else false
//it will take that randomly generated otp as input parameter
//otp is generated randomly in login section of this code
bool verifyOTP(String otp) {
  int otp1 = otp.toInt();// converting string to int
  int digitotp1 = otp1 / 10; //getting individual digits of otp generated for further crosschecking
  int digitotp2 = otp1 % 10;
  //bool flag = true;
  //  int count = 0; //in case of login we just need 2 digits... more than 2 not accepted
  int d[2];//for storing digits of otp entered and we will crosscheck it with digitotp1 and digitotp2
  Serial.print("OTP is:");
  for (int j = 0; j < 2; j++)
  {
    int t = getdigit() ;
    d[j] = t;
    if ( t !=  -1 )
      Serial.print(t );
    t = -1 ;
    delay(3000);
  }//this loop accepts and prints only 2 touch pin readings
  //because our otp is of 2 digits
  if (d[0] == digitotp1 && d[1] == digitotp2) {
    return true;
  }//verifying otp
  else {
    return false;
  }//in case of wrong otp we will give user another chance
  //to re enter the otp . we will do this in the login section below
}


int no_of_2000notesleft = 5;
int no_of_1000notesleft = 10;
int no_of_500notesleft = 10;
int availablebal = 25000; //stored this variable in eeprom, this variable will be updated constantly and will let us know final available balance

//this is function for letting user know available denominations, taking denominations user wants to withdraw as input through touch pins ,
//printing the requested denominations,amount entered for withdrawl, and finally balance available in your account.
int deno2000 = 0;
int deno1000 = 0;
int deno500 = 0;
int amount = 0;
void withdrawAmount() {

  //delay(2000);
  Serial.println("\nNow start pressing the touchpins for denominations you want");
  //bot.sendMessage(chat_id, "Now start pressing the touchpins for denominations you want", "");
  while (true) {
    int t = getdigit();
    delay(3000);
    if (t == 3) {
      Serial.println("As you have pressed touch3, you confirmed the above denominations");
      //bot.sendMessage(chat_id, "As you have pressed touch3, you confirmed the above denominations", "");
      break;
    }
    else {
      if (t == 6) {
        Serial.println("\nYou just requested for 1 denomination of Rs.2000");
        //bot.sendMessage(chat_id, "You just requested for 1 denomination of Rs.2000", "");
        deno2000++;
      }
      else if (t == 5) {
        Serial.println("\nYou just requested for 1 denomination of Rs.1000");
        //bot.sendMessage(chat_id, "You just requested for 1 denomination of Rs.1000", "");
        deno1000++;
      }
      if (t == 4) {
        Serial.println("\nYou just requested for 1 denomination of Rs.500");
        //bot.sendMessage(chat_id, "You just requested for 1 denomination of Rs.500", "");
        deno500++;
      }
    }
  }
  Serial.println("Denominations you have chosen are: ");
  Serial.print("Rs.2000 notes :");
  Serial.print(deno2000);
  Serial.print("Rs.1000 notes :");
  Serial.print(deno1000);
  Serial.print("Rs. 500 notes :");
  Serial.print(deno500);



  if (deno2000 <= no_of_2000notesleft && deno1000 <= no_of_1000notesleft && deno500 <= no_of_500notesleft) {
    Serial.println("Amount entered is valid");
    //bot.sendMessage(chat_id, "Amount entered is valid", "");
    amount = 2000 * deno2000 + 1000 * deno1000 + 500 * deno500;
    Serial.println("Amount is : ");
    Serial.println(amount);


    no_of_2000notesleft = no_of_2000notesleft - deno2000;
    no_of_1000notesleft = no_of_1000notesleft - deno1000;
    no_of_500notesleft = no_of_500notesleft - deno500;
    availablebal = availablebal - amount;

    Serial.println("Amount is successfully withdrawn. Thank you for choosing our ATM service ");

    //and available balance is:
    //Serial.println(availablebal);
  }
  else {
    Serial.println("The denominations entered doesn't satisfy available number of notes");
    //bot.sendMessage(chat_id, "The denominations entered doesn't satisfy available number of notes", "");
    Serial.println("You may re-enter the amount. Press touch7 to reenter and any other touch pin to exit");
    //bot.sendMessage(chat_id, "You may re-enter the amount. Press touch7 to reenter and any other touch pin to exit", "");
    if (getdigit() == 7)
      withdrawAmount();
    return;
  }
  return;
}

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("Handling New Message");
  Serial.println(String(numNewMessages));
  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    // Print the received message
    String user_text = bot.messages[i].text;
    Serial.println(user_text);
    String your_name = bot.messages[i].from_name;

    //start command
    if (user_text == "/start") {
      String welcome = "Welcome, " + your_name + ".\n";
      welcome += "Use the following commands to control the ESP32 based ATM system.\n\n";
      welcome += "Send /login to login into the system and after successfully logging in you will be able to withdraw the amount\n";
      welcome += "Send /balance to know the current account balance . It will tell you balance as well as demonitions left \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    //login command
    if (user_text == "/login") {
      String otp = String(random(100));
      
      Serial.println("\nPlease enter the 2 digit OTP displayed below using touchpins");
      bot.sendMessage(chat_id, "Please enter the 2 digit OTP displayed below using touchpins", "");
      Serial.println("otp");
      bot.sendMessage(chat_id, otp, "");
      bool ver = verifyOTP(otp); //here we will display and verify otp
      for (;;) {
        if (ver == true) {
          Serial.println("OTP entered is correct");
          bot.sendMessage(chat_id, "OTP entered is correct", "");
          break;
        }
        else {
          Serial.println("OTP you entered is not correct.Please Re-enter the otp");
          bot.sendMessage(chat_id, "OTP you entered is not correct.Please Re-enter the otp", "");
          ver = verifyOTP(otp);
        }//facility for re-entering the otp incase we submitted wrong otp
      }
      Serial.println("Now you can enter the denominations of amount you want to withdraw");
      bot.sendMessage(chat_id, "Now you can enter the denominations of amount you want to withdraw", "");

      Serial.println("Available notes are : ");
      Serial.print("Rs.2000 notes :");
      Serial.print(no_of_2000notesleft);
      Serial.print("Rs.1000 notes :");
      Serial.print(no_of_1000notesleft);
      Serial.print("Rs. 500 notes :");
      Serial.print(no_of_500notesleft);
      Serial.println("\n>>Press touch6 once to withdraw a single Rs.2000 note ");
      Serial.println("\n>>Press touch5 once to withdraw a single Rs.1000 note ");
      Serial.println("\n>>Press touch4 once to withdraw a single Rs.500 note");
      Serial.println("\nMake sure number you enter is less than or eqaual to notes available");
      Serial.println("\n>>Press touchpin3 if you are done with entering the denominations");//taking confirmation from user

      bot.sendMessage(chat_id, "Available notes are : ", "");
      bot.sendMessage(chat_id, "Rs.2000 notes :", "");
      bot.sendMessage(chat_id, String(no_of_2000notesleft), "");
      bot.sendMessage(chat_id, "Rs.1000 notes :", "");
      bot.sendMessage(chat_id, String(no_of_1000notesleft), "");
      bot.sendMessage(chat_id, "Rs.500 notes :", "");
      bot.sendMessage(chat_id, String(no_of_500notesleft), "");
      bot.sendMessage(chat_id, ">>Press touch6 once to withdraw a single Rs.2000 note ", "");
      bot.sendMessage(chat_id, ">>Press touch5 once to withdraw a single Rs.1000 note ", "");
      bot.sendMessage(chat_id, ">>Press touch4 once to withdraw a single Rs.500 note ", "");
      bot.sendMessage(chat_id, "Make sure number you enter is less than or eqaual to notes available", "");
      bot.sendMessage(chat_id, ">>Press touchpin3 if you are done with entering the denominations", "");

      withdrawAmount();//here we will enter amount using touch pins, then we will validate it, and then we will display it

      bot.sendMessage(chat_id, "Denominations you have chosen are: ", "");
      bot.sendMessage(chat_id, "Rs.2000 notes :", "");
      bot.sendMessage(chat_id, String(deno2000), "");
      bot.sendMessage(chat_id, "Rs.1000 notes :", "");
      bot.sendMessage(chat_id, String(deno1000), "");
      bot.sendMessage(chat_id, "Rs.500 notes :", "");
      bot.sendMessage(chat_id, String(deno500), "");

      bot.sendMessage(chat_id, "Amount is : ", "");
      bot.sendMessage(chat_id, String(amount), "");
      bot.sendMessage(chat_id, "Amount is successfully withdrawn. Thank you for choosing our ATM service ", "");

      Serial.println("Press /balance to know your current account balance");
      bot.sendMessage(chat_id, "Press /balance to know your current account balance", "");
    }//login ends here

    //BALANCE COMMAND
    if (user_text == "/balance") {
      bot.sendMessage(chat_id, "Your current account balance is as follows", "");
      Serial.println("Now the available balance is :");//printing balance on serial monitor
      Serial.println(availablebal);
      Serial.println("Available denominations are : ");
      Serial.println("Number of 2000Rs notes left :");
      Serial.print(no_of_2000notesleft);
      Serial.println("Number of 1000Rs notes left :");
      Serial.println(no_of_1000notesleft);
      Serial.println("Number of 500Rs notes left :");
      Serial.println(no_of_500notesleft);
      Serial.println("Thank you");

      //printing balance on tele bot
      bot.sendMessage(chat_id, "Now the available balance is :", "");
      bot.sendMessage(chat_id, String(availablebal), "");
      bot.sendMessage(chat_id, "Available denominations are : ", "");
      bot.sendMessage(chat_id, "Number of 2000Rs notes left :", "");
      bot.sendMessage(chat_id, String(no_of_2000notesleft), "");
      bot.sendMessage(chat_id, "Number of 1000Rs notes left :", "");
      bot.sendMessage(chat_id, String(no_of_1000notesleft), "");
      bot.sendMessage(chat_id, "Number of 500Rs notes left :", "");
      bot.sendMessage(chat_id, String(no_of_500notesleft), "");
      bot.sendMessage(chat_id, "Thank you", "");
    }
  }
}

void setup() {
  Serial.begin(115200);
  //setting pinmode
  pinMode(4, INPUT);
  pinMode(2, INPUT);
  pinMode(15, INPUT);
  pinMode(13, INPUT);
  pinMode(12, INPUT);
  pinMode(14, INPUT);
  pinMode(27, INPUT);
  pinMode(33, INPUT);
  pinMode(32, INPUT);

  // Connecting to Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid , password);

  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  ThingSpeak.begin(client);  // Initializing ThingSpeak
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32's Local IP Address on serial monitor
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + bot_delay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("Got Response!");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    // Calling balance amount and denominations of Rs.2000/-, Rs.1000/-  and Rs500/- from EEPROM to plot on Thingspeak
    //    int balanceAmount = EEPROM.get(1,eepromArr[0])*100;
    //    int Denomination2000= EEPROM.get(2,eepromArr[1]);
    //    int Denomination1000=EEPROM.get(3,eepromArr[2]);
    //    int Denomination500=EEPROM.get(4,eepromArr[3]);

    String DataSent1 = "api_key=" + apiKey1 + "&field1=" + String(availablebal); //Link to access numerical wideget of balance Amount
    http.POST(DataSent1); //Sending balanceAmount values to Thingspeak
    delay(50);
    String DataSent2 = "api_key=" + apiKey2 + "&field1=" + String(deno2000); //Link to access numerical wideget of Denomination of 2000
    http.POST(DataSent2);//Sending 2000 Denomination values to Thingspeak
    delay(50);
    String DataSent3 = "api_key=" + apiKey3 + "&field1=" + String(deno1000); //Link to access numerical wideget of Denomination of 1000
    http.POST(DataSent3);//Sending 1000 Denomination values to Thingspeak
    delay(50);
    String DataSent4 = "api_key=" + apiKey4 + "&field1=" + String(deno500);  //Link to access numerical wideget of Denomination of 500
    http.POST(DataSent4);//Sending 500 Denomination values to Thingspeak
    delay(50);

//    //Printing the values on Serial Monitor
//    Serial.print("\nBalance Amount: ");
//    Serial.print(balanceAmount);
//    Serial.print("\nDenomination of 2000: ");
//    Serial.print(Denomination2000);
//    Serial.print("\nDenomination of 1000: ");
//    Serial.print(Denomination1000);
//    Serial.print("\nDenomination of 500: ");
//    Serial.print(Denomination500);
    http.end();
  }
}
  //    //displaying no. of notes left on thingspeak's numerical widget
  //    int x1 = ThingSpeak.writeField(myChannelNumber, 1, no_of_2000notesleft, myWriteAPIKey);
  //    if (x1 == 200) {
  //      Serial.println("Thingspeak Channel 2000 update successful.");
  //    }
  //    else {
  //      Serial.println("Problem updating thingspeak 2000 channel. HTTP error code " + String(x1));
  //      delay(5000);
  //    }
  //
  //    int x2 = ThingSpeak.writeField(myChannelNumber, 1, no_of_1000notesleft, myWriteAPIKey);
  //    if (x2 == 200) {
  //      Serial.println("Thingspeak Channel 1000 update successful.");
  //    }
  //    else {
  //      Serial.println("Problem updating thingspeak 1000 channel. HTTP error code " + String(x2));
  //      delay(5000);
  //    }
  //
  //    int x3 = ThingSpeak.writeField(myChannelNumber, 1, no_of_500notesleft, myWriteAPIKey);
  //    if (x3 == 200) {
  //      Serial.println("Thingspeak Channel 500 update successful.");
  //    }
  //    else {
  //      Serial.println("Problem updating thingspeak 500 channel. HTTP error code " + String(x3));
  //      delay(5000);
  //
  //      lastTimeBotRan = millis();
  //    }
