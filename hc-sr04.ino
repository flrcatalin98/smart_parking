//---- DEFINE
#define NUM_SENZORI 2  // Numarul de senzori din sistemul de parcare inteligenta
#define MIN_CM 4       // Distanta de la care senzorul considera locul ca fiind ocupat
#define MAX_CM 16      // Distanta de la care senzorul nu mai considera locul ca fiind ocupat
#define DELAY_SEC 1    // Timpul de asteptare in milisecunde pana la urmatoarea verificare
#define BAUD 9600      // Data rate-ul portului COM4

//---- PINURI
int trigPin[NUM_SENZORI];
int echoPin[NUM_SENZORI];

//---- setup()
// Este apelata o singura data de fiecare data cand este uploadat un sketch
void setup() {

  int count;

  count = 2;                              //----
  for(int i = 0; i < NUM_SENZORI; i++) {  // 
    trigPin[i] = count;                   // Declar pinurile Trig ca fiind cele pare
    count = count + 2;                    // incepand de la pinul 2
  }                                       //----

  count = 3;                              //----
  for(int i = 0; i < NUM_SENZORI; i++) {  //
    echoPin[i] = count;                   // Declar pinurile Echo ca fiind cele impare
    count = count + 2;                    // incepand de la pinul 3
  }                                       //----

  Serial.begin(BAUD);                     // Setez data rate-ul portului COM4
  
  for(int i = 0; i < NUM_SENZORI; i++) {  //----
    pinMode(trigPin[i], OUTPUT);          // Setez pinurile Trig ca fiind pinuri de output 
    pinMode(echoPin[i], INPUT);           // Setez pinurile Echo ca fiind pinuri de input
  }                                       //----
  
}

//---- getDurata()
// Returneaza durata de timp in care semnalul ultrasonic trimis de senzor se reflecta inapoi la acesta
int* getDurata() {

  static int durata[NUM_SENZORI];

  for(int i = 0; i < NUM_SENZORI; i++) {                    //----
    digitalWrite(trigPin[i], LOW); delayMicroseconds(5);    // Dau un pulse LOW de scurta durata pentru a asigura un pulse HIGH curat
    digitalWrite(trigPin[i], HIGH); delayMicroseconds(10);  // Senzorul este pornit de un pulse HIGH de 10 sau mai multe microsecunde
    digitalWrite(trigPin[i], LOW);                          //
                                                            //
    durata[i] = pulseIn(echoPin[i], HIGH);                  // Pornesc timerul pentru a calcula distanta
  }                                                         //----

  return durata;
  
}

//---- getCm()
// Returneaza distanta in centimetrii pana la obiectul detectat de senzor
int* getCm() {

  int* durata = getDurata();
  static int cm[NUM_SENZORI];

  for(int i = 0; i < NUM_SENZORI; i++) {  //----
    cm[i] = (durata[i]/2) / 29.1;         // Calculul distantei in centimetrii conform formulei din documentata senzorului
  }                                       //----

  return cm;
  
}

//---- getSstatus()
// Returneaza statusul senzorilor
boolean* getSstatus() {

  int* cm = getCm();
  static boolean sstatus[NUM_SENZORI];

  for(int i = 0; i < NUM_SENZORI; i++) {      //----
    if(cm[i] >= MIN_CM && cm[i] <= MAX_CM) {  // Daca un obiect se afla in intervalul definit de MIN_CM si MAX_CM
      sstatus[i] = true;                      // statusul este considerat 'true'
    } else {                                  // iar in caz contrar
      sstatus[i] = false;                     // acesta este considerat 'false'
    }                                         //
  }                                           //----

  return sstatus;
  
}

//---- printSstatus()
// Afiseaza in terminalul serial (COM4 - 9600baud) statusul senzorilor
void printSstatus() {

  boolean* sstatus = getSstatus();

  Serial.write(27);                       //----
  Serial.print("[2J");                    // Refresh terminal
  Serial.write(27);                       //
  Serial.print("[H");                     //----

  Serial.print(millis() / 1000);          //----
  Serial.println(" sec");                 // Afisez timpul in secunde de la inceputul executiei
  Serial.println();                       //----
  
  Serial.print("NR.");                    //----
  Serial.print('\t');;                    // Printez numele coloanelor
  Serial.println("STATUS");               //----
    
  for(int i = 0; i < NUM_SENZORI; i++) {  //----
    Serial.print(i + 1);                  // Printez ID-ul senzorului
    Serial.print('\t');                   //
    if(sstatus[i]) {                      // Daca statusul senzorului este 'true'
      Serial.println("Ocupat");           // locul de parcare este considerat ca fiind ocupat
    } else {                              // iar in caz contrar
      Serial.println("Liber");            // acesta este considera ca fiind liber
    }                                     //
  }                                       //----

  delay(DELAY_SEC * 1000);                // Timpul de asteptare in milisecunde pana la urmatoarea verificare
  
}

//---- loop()
// Se apeleaza pe toata durata executiei
void loop() {

  printSstatus();
  
}
