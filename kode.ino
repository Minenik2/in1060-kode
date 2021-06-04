#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define echoPin 9
#define trigPin 10

char *utfordringer[] = {"Gjett 20cm avstand", "Skyt"};
char *utfordringerLang[] = {"Gjett 20cm avstand",
                                  "Hink lengre enn venstremann",
                                  "Skyt eldste spiller på 10 meter",
                                  "Sta pa henda i 10sek",
                                  "Dobbelt poeng pa neste utfordring",
                                  "+1 poeng til venstremann ved feil pa neste utfordring",
                                  "Sta 1 meter unna sensor",
                                  "Skyt i varet og fang skuddet",
                                  "20 push-ups",
                                  "30 squats",
                                  "Gjor 1 pistol squat",
                                  "Forste til a skyte med hoyrehand",
                                  "Kast 3 terninger hver med hoyremann, den med høyest samlet sum vinner",
                                  "Skyt ned en brikke fra 10 meter",
                                  "Skyt spiller til venstre fra 5 meter imens han/hun dukker unna"
                                 };
int currentUtfordringIndex;
char* aktuellUtfordring;
long aktuellHinkeLengde;
long naverendeAvstand;
const int knappPin = 8;
boolean klarteDetEllerIkke = false;
int forsteAvstand;
int andreAvstand;

//# alt for å fikse poeng og motorer
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
//#

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

  randomSeed(666);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Velkommen!");
}

void loop() {
  // om knapp trykkes vis en utfordring
  if (digitalRead(knappPin) == HIGH) {
 	visUtfordring();
  }
  // autoscroll funksjon
  const char* str = " ";
  lcd.setCursor(11, 0);
  lcd.autoscroll();
  lcd.print(str[0]);
  delay(400);
  // skjekker om en spiller har trykket på +poeng knappen
  checkPlayerPoeng();
}

// leser inn hvem som trykket på motorknapper og gir den spilleren poeng
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

// skriver noe i bunn av skjermen
void skrivLCD(char* text) {
    lcd.setCursor(0, 1);
    lcd.print(text);
    delay(800);
    int textLengde = strlen(text);
    // hvis setningen er for lang, vil den scrolle
    if (textLengde > 15) {
        char* str2 = text;
        lcd.setCursor(0, 1);
        lcd.autoscroll();
        for (int i = 0; i < textLengde - 10; i++)
        {
          lcd.print(str2[i]);
          checkPlayerPoeng();
          delay(400);
        }
      }
      lcd.noAutoscroll();
  }

// skriver utfordringen på lcd & utfører noen spesielle utfordringer
void visUtfordring() {
	lcd.noAutoscroll();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Utfordringen:");
    currentUtfordringIndex = random(sizeof(utfordringerLang) / sizeof(char*));
    //currentUtfordringIndex = 0;
    aktuellUtfordring = utfordringerLang[currentUtfordringIndex];
    skrivLCD(aktuellUtfordring);
    Serial.print("currentUtfordringIndex, altså nr i arrayen, er: ");
    Serial.println(currentUtfordringIndex);
    Serial.print("Og det er utfordringen: ");
    Serial.println(aktuellUtfordring);

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
      lcd.setCursor(0, 0);
      lcd.print("Forstemann hinker");
      forsteAvstand = hinkeLengde(140);
      lcd.clear();
      lcd.print("Andremann hinker");
      andreAvstand = hinkeLengde(140);
      if (forsteAvstand < andreAvstand) {
        lcd.clear();
        lcd.print("Forstemann vant!");
      }
      else if (forsteAvstand > andreAvstand) {
        lcd.clear();
        lcd.print("Andremann vant!");
      }

    } else if (aktuellUtfordring == utfordringerLang[6]) {
        // Sta 1 meter unna sensor
        avstandsGjetter(100);
      }
}

// sjekker om noen av spillerne klikket på knappen
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

// måle avstand og sende tilbake true eller false om avstanden er lik gjetteMal
boolean avstandsGjetter(long gjetteMal) {
  lcd.noAutoscroll();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Maler om:");
  for (int i = 7; i > 0; i--) {
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
      for (int i = 0; i < 5; i ++) {
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

// maler avstanden og returnerer den
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

// Måler avstand av en spiller 2 ganger og viser hvor langdt spilleren bevegde seg
int hinkeLengde(int onsketAvstand) {
	 lcd.noAutoscroll();
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
	// hvis "naverendeAvstand == onsketAvstanddet" så vil sensoren starte å måle på om 4 sekunder 
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
	    lcd.print(i);
	    lcd.print(" sek");
	    delay(700);
	  }
	  aktuellHinkeLengde = avstandsMaaler();
	
	// skriver ut på lcd skjermen hvor mye brukeren hinket
	  lcd.clear();
	  lcd.setCursor(0, 0);
	  lcd.print("Du hinket: ");
	  lcd.setCursor(0, 1);
	  lcd.print(onsketAvstand - aktuellHinkeLengde);
	  delay(3000);
	  return onsketAvstand - aktuellHinkeLengde;
}
