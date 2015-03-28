/* Knight Rider 2
 * --------------
 *
 * Reducing the amount of code using for(;;).
 *
 *
 * (cleft) 2005 K3, Malmo University
 * @author: David Cuartielles
 * @hardware: David Cuartielles, Aaron Hallborg
 */

/* use more descriptive names for pin assignment */
#define LED1    2
#define LED2    3
#define LED3    4
#define LED4    5
#define LED5    6
#define LED6    7

int pinArray[] = {PIN1, PIN2, PIN3, PIN4, PIN5, PIN6};
#define MAX_LEDS    ( sizeof(pinArray) / sizeof(pinArray[0]) )
int timer = 100;

void setup(){
  int count = 0;
  // we make all the declarations at once
  for (count=0;count<MAX_LEDS;count++) {
    pinMode(pinArray[count], OUTPUT);
  }
}

void loop() {
  int count = 0;
  for (count=0;count<MAX_LEDS;count++) {
   digitalWrite(pinArray[count], HIGH);
   delay(timer);
   digitalWrite(pinArray[count], LOW);
   delay(timer);
  }
  for (count=MAX_LEDS-1;count>=0;count--) {
   digitalWrite(pinArray[count], HIGH);
   delay(timer);
   digitalWrite(pinArray[count], LOW);
   delay(timer);
  }
}


