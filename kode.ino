#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(11, 12, 2, 3, 4, 5);
// alt for å fikse poeng og motorer
const int PLAYER1_BUTTON = A4;
const int PLAYER2_BUTTON = A5;
const int PLAYER3_BUTTON = A3;

const int PLAYER1_SERVO_PIN = 6;
const int PLAYER2_SERVO_PIN = 13;
const int PLAYER3_SERVO_PIN = 7;

// player poeng
int player1poeng = 0;
int player2poeng = 0;
int player3poeng = 0;
int player4poeng = 0;

// player motor
Servo PLAYER1_SERVO;
Servo PLAYER2_SERVO;
Servo PLAYER3_SERVO;
Servo PLAYER4_SERVO;

Servo currentServo;
//

#define echoPin 9
#define trigPin 10

const char *utfordringer[] = {"Gjett 20cm avstand", "Skyt"};
const char *utfordringerLang[] = {"Gjett 20cm avstand",
                                  "Hink lengre enn venstremann",
                                  "Skyt eldste spiller på 10 meter",
                                  "Sta pa henda i 10sek",
                                  "Dobbelt poeng pa neste utfordring",
                                  "-1 poeng ved feil pa neste utfordring",
                                  "Sta 1 meter unna sensor",
                                  "Skyt i varet og fang skuddet",
                                  "20 push-ups",
                                  "30 squats",
                                  "Gjor 1 pistol squat",
                                  "Forste til a skyte med hoyrehand",
                                  "Kast 3 terninger hver med hoyremann, den med høyest samlet sum vinner",
                                  "Skyt ned en brikke fra 10 meter, sett brikke foran sensor",
                                  "Skyt spiller til venstre fra 5 meter imens han/hun dukker unna"
                                 };
long currentUtfordring;
const char *aktuellUtfordring;
long aktuellHinkeLengde;
long naverendeAvstand;
const int knappPin = 8;
boolean klarteDetEllerIkke = false;
int forsteAvstand;
int andreAvstand;
int lengde;

int vinkelVenstre;
int vinkelMidt = 0;
int vinkelHoyre;
int poengVenstre = 0;
int poengMidt = 0;
int poengHoyre = 0;
const char* hoyreNavn = "hoyre";
const char* midtNavn = "midt";
const char* venstreNavn = "venstre";

void setup() {
  pinMode(knappPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // setter opp motorer og playerpoeng
  pinMode(PLAYER1_BUTTON, INPUT);
  pinMode(PLAYER2_BUTTON, INPUT);
  pinMode(PLAYER3_BUTTON, INPUT);
  
  PLAYER1_SERVO.attach(PLAYER1_SERVO_PIN, 500, 2500);
  PLAYER2_SERVO.attach(PLAYER2_SERVO_PIN, 500, 2500);
  PLAYER3_SERVO.attach(PLAYER3_SERVO_PIN, 500, 2500);
  
  PLAYER1_SERVO.write(0);
  PLAYER2_SERVO.write(0);
  PLAYER3_SERVO.write(0);
  PLAYER4_SERVO.write(0);
  //

  randomSeed(analogRead(0));
  Serial.begin(9600);
  vinkelVenstre = map(poengVenstre, 0, 5, 0, 179);
  vinkelHoyre = map(poengHoyre, 0, 5, 0, 179);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Velkommen!");
  delay(2000);
}

void loop() {
  // om knapp trykkes, start showet!
  if (digitalRead(knappPin) == HIGH) {
    lcd.noAutoscroll();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Utfordringen:");
    currentUtfordring = random(sizeof(utfordringerLang) / sizeof(char*));
    //currentUtfordring = 0;
    aktuellUtfordring = utfordringerLang[currentUtfordring];
    skrivLCD(aktuellUtfordring);
    delay(500);/*
    lcd.setCursor(0, 1);
    lcd.print(aktuellUtfordring);
    int aktuellUtfordringLengde = strlen(aktuellUtfordringLengde);
    if (aktuellUtfordringLengde > 6) {
      const char* str = "     ";
      // hvis setningen er for lang, vil den scrolle
        lcd.setCursor(11, 0);
        lcd.autoscroll();
        for (int i = 0; i < strlen(str); i++)
        {
          lcd.print(str[i]);
          checkPlayerPoeng();
          delay(400);
        }
      }*/
    Serial.print("CurrentUtfordring, altså nr i arrayen, er: ");
    Serial.println(currentUtfordring);
    Serial.print("Og det er utfordringen: ");
    Serial.println(aktuellUtfordring);
    delay(3000);
    /*0"Gjett 20cm avstand", 1"Hink lengre enn venstremann", 2"Skyt eldste spiller på 10 meter",
      3"Sta på henda i 10sek", 4"Dobbelt poeng på neste utfordring",
      5"-1 poeng ved feil på neste utfordring", 6"Sta 1 meter unna sensor",
      7"Skyt i varet og fang skuddet", 8"20 push-ups",
      9"30 squats", 10"Gjor 1 pistol squat", 11"Forste til a skyte med hoyrehand"*/

    if (aktuellUtfordring == utfordringerLang[0]) {
      // om tildelt utfordring er 20 cm avstand
      klarteDetEllerIkke = avstandsGjetter(20);
      if (klarteDetEllerIkke == false) {
        skrivLCD("Et forsok til!");
        avstandsGjetter(20);
      }
      skrivLCD("Velg ny utfordring!");
    }
    else if (aktuellUtfordring == utfordringerLang[1]) {
      // hink lengre enn andremann
      // male til avstand er 140 cmmeter
      // måle først at de står x cmeter unna, så hoppe og måle hva avstand blir
      lcd.clear();
      lcd.print("Forstemann hinker");
      forsteAvstand = hinkeLengde(140);
      lcd.clear();
      lcd.print("Andremann hinker");
      andreAvstand = hinkeLengde(140);
      if (forsteAvstand < andreAvstand) {
        lcd.clear();
        lcd.print("Forstemann vant!");
        //faaPoeng(1, 1); lar brukeren klikke på knappen selv
      }
      else if (forsteAvstand > andreAvstand) {
        lcd.clear();
        lcd.print("Andremann vant!");
        //faaPoeng(2, 1);
      }
      
    } else if (aktuellUtfordring == utfordringerLang[6]) {
        // måle 1 m avstand
        avstandsGjetter(100);
      }
  }
  const char* str = "     ";
      // hvis setningen er for lang, vil den scrolle
        lcd.setCursor(11, 0);
        lcd.autoscroll();
        for (int i = 0; i < strlen(str); i++)
        {
          lcd.print(str[i]);
          checkPlayerPoeng();
          delay(400);
        }
   checkPlayerPoeng();
}

int readButtonPress(int playerButton, int playerPoeng) {
  if (playerButton == PLAYER1_BUTTON) {
    currentServo = PLAYER1_SERVO;
  } else if (playerButton == PLAYER2_BUTTON) {
    currentServo = PLAYER2_SERVO;
  } else {
    currentServo = PLAYER3_SERVO;
  }
  if (playerPoeng <= 5) {
      playerPoeng++;
      for (int i = 0; i < playerPoeng; i++) {
        int rotate = 37 * (i + 1);
        if (rotate > 180) {
          rotate = 180;
        }
        currentServo.write(37 * (i + 1));
      }
      delay(200);
    }
  return playerPoeng;
}

void skrivLCD(char* text) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(text);
    delay(800);
    int textLengde = strlen(text);
    // hvis setningen er for lang, vil den scrolle
    if (textLengde > 6) {
        const char* str2 = text;
        //const char* str2 = "         "; // "     "
        //lcd.setCursor(11, 0);
        lcd.setCursor(0, 1);
        lcd.autoscroll();
        for (int i = 0; i < strlen(str2) - 5; i++)
        {
          lcd.print(str2[i]);
          checkPlayerPoeng();
          delay(400);
        }
      }
      lcd.noAutoscroll();
  }

void checkPlayerPoeng() {
    if (digitalRead(PLAYER1_BUTTON) != LOW) {
   player1poeng = readButtonPress(PLAYER1_BUTTON, player1poeng);
   skrivLCD("Spiller 1 fikk poeng!");
  }
  if (digitalRead(PLAYER2_BUTTON) != LOW) {
    player2poeng = readButtonPress(PLAYER2_BUTTON, player2poeng);
    skrivLCD("Spiller 2 fikk poeng!");
  }
  if (digitalRead(PLAYER3_BUTTON) != LOW) {
    player3poeng = readButtonPress(PLAYER3_BUTTON, player3poeng);
    skrivLCD("Spiller 3 fikk poeng!");
  }
  }

// måle avstand og sende tilbake avstanden
boolean avstandsGjetter(long gjetteMal) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Maler om:");
  for (int i = 7; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print("         ");
    lcd.setCursor(0, 1);
    lcd.print(i);
    lcd.print(" sek");
    delay(700);
  }
  long varighet, avstand;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  varighet = pulseIn(echoPin, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  // dersom sensoren oppfatter noe
  if (varighet > 0) {
    avstand = (varighet / 2) / 29.1;
    Serial.println(avstand);
    lcd.print("Avstanden er:");
    lcd.setCursor(0, 1);
    lcd.print(avstand);
    lcd.print(" cm");
    delay(1000);
    lcd.clear();

    // om avstanden er over gjetteMal cm
    if (avstand > gjetteMal) {
      lcd.print("Mer enn malet");
      return false;
    }
    else if (avstand == gjetteMal) {
      for (int i = 0; i < 10; i ++) {
        lcd.print("Gratulerer!");
        lcd.setCursor(0, 1);
        lcd.print("Akkurat malet!!");
        delay(1000);
        lcd.clear();
        delay(500);
      }
      return true;
    }
    // om avstanden ikke er over 20 cm
    else if (avstand < gjetteMal) {
      lcd.print("Mindre enn malet");
      return false;
    }
  }
  // dersom sensoren ikke oppdager noe
  else {
    lcd.print("Ingenting foran");
    Serial.println("The pulse never hit anything...");
    delay(2000);
  }
}

long avstandsMaaler() {
  long varighet, avstand;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  varighet = pulseIn(echoPin, HIGH);
  if (varighet > 0) {
    avstand = (varighet / 2) / 29.1;
    Serial.println(avstand);
    return avstand;
  }
  else {
    Serial.println("Pulsen traff ingenting...");
    return 0;
  }
}

int hinkeLengde(int onsketAvstand) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ga ");
  lcd.print(onsketAvstand);
  lcd.print(" cm unna");

  delay(2000);

  naverendeAvstand = avstandsMaaler();

  while (naverendeAvstand != onsketAvstand) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Avstanden er:");
    lcd.setCursor(0, 1);
    lcd.print(naverendeAvstand);
    lcd.print(" cm");
    delay(1000);
    naverendeAvstand = avstandsMaaler();
  }
  Serial.print("Dette er naverendeAvstand etter at den har blitt større enn onsketAvstand: ");
  Serial.println(naverendeAvstand);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hink na!");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Maler om:");
  for (int i = 4; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print("         ");
    lcd.setCursor(0, 1);
    lcd.print(i);
    lcd.print(" sek");
    delay(700);
  }
  aktuellHinkeLengde = avstandsMaaler();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Du hinket: ");
  lcd.setCursor(0, 1);
  lcd.print(onsketAvstand - aktuellHinkeLengde);
  delay(3000);
  return onsketAvstand - aktuellHinkeLengde;
}
