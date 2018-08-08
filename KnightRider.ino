/* Move LED like a shift register
 * --------------
 *
 * function:
 *
 * use a byte(8bit) variable LEDS to store the state of the 8 LEDS
 * LED assignment from LED7..LED0 assigned bit 2^8..2^0
 * We have pins for shift-in(INPUT) and a shift-out(OUTPUT)
 * 1. read shift-in pin
 * 2. shift
 *
 * @author: Heinz Blaettner
 * @hardware: e.g. Arduino UNO
 */

// Here you can configure how the LEDs are wired to the pins
// Set this configuration to keep behaviour of LEDs independent of wiring
// otherwise the logical bit value and LEDs on/off are inverted
// for HIGH_ACTIVE  the wiring is pin - resistor - LED - GND
// for LOW_ACTIVE   the wiring is VCC - resistor - LED - pin
//#define LED_HIGH_ACTIVE    true
#define LED_HIGH_ACTIVE    false

// This configuration control the direction of the shift left/right
#define SHIFT_DIR_LEFT      true
#define SHIFT_DIR_RIGHT     false
//#define SHIFT_DIR           SHIFT_DIR_RIGHT
#define SHIFT_DIR           SHIFT_DIR_LEFT

/* use more descriptive names for pin assignment */
#define PIN_LED0        2
#define PIN_LED1        3
#define PIN_LED2        4
#define PIN_LED3        5
#define PIN_LED4        6
#define PIN_LED5        7
#define PIN_LED6        8
#define PIN_LED7        9
#define PIN_LED_OUT     10
#define PIN_LED_IN      11
#define PIN_LED_IN_THRESHOLD    1       // number of loops PIN_LED_IN must be HIGH

//#define SERIAL_SPEED    9600
#define SERIAL_SPEED    115200

uint16_t LEDS   = 0xff;     // START WITH ALL LEDW
int LEDArray[]  = {
    PIN_LED0,
    PIN_LED1,
    PIN_LED2,
    PIN_LED3,
    PIN_LED4,
    PIN_LED5,
    PIN_LED6,
    PIN_LED7
};
#define MAX_LEDS    ( sizeof(LEDArray) / sizeof(LEDArray[0]) )

int timer = 200;

// ====================================
void setup(){
    int i = 0;

    Serial.begin(SERIAL_SPEED);
    Serial.println("KnightRider: shift");
    Serial.print("MAX_LEDS=        " ); Serial.println( MAX_LEDS );
    Serial.print("SHIFT_DIR=       " ); Serial.println( (SHIFT_DIR_LEFT==SHIFT_DIR)? "LEFT" : "RIGHT" );
    Serial.print("LED_HIGH_ACTIVE= " ); Serial.println( (LED_HIGH_ACTIVE)    ? "true" : "false" );
    Serial.print("PIN_LED_IN=      " ); Serial.println( PIN_LED_IN  );
    Serial.print("PIN_LED_OUT=     " ); Serial.println( PIN_LED_OUT );

    // setup pins for shift-in shift-out
    pinMode( PIN_LED_OUT, OUTPUT);
    pinMode( PIN_LED_IN,  INPUT);

    // Setup all LED pins to output
    for (i=0; i<MAX_LEDS; i++) {
        pinMode( LEDArray[i], OUTPUT);
        Serial.print("PIN_LED" ); Serial.print(i );
        Serial.print("=        " ); Serial.println( LEDArray[i] );
    }
}

// ====================================
void update_LEDS( unsigned leds) {
    int i     = 0;
    int mask  = 1<<(MAX_LEDS-1);    // for extract/check pit at position i
    int bv    = 0;                  // bit value

    Serial.print("LEDS=");
    Serial.print(leds, HEX);
    Serial.print("=<");

    for (i=MAX_LEDS-1; i>=0; i--, mask >>= 1) {
        bv = (leds & mask) ? HIGH : LOW;
        Serial.print(bv);
        bv = (LED_HIGH_ACTIVE) ? bv : !bv ;    // Invert signal for output in case of LOW_ACTIVE
        digitalWrite(LEDArray[ i ], bv);
    }
    Serial.print(">");
}

// ====================================
/*
 * int *pShift_val  - ptr to var which contain bit values
 * int shift_in     - the logical level will be shifted in depending on direction
 * int shift_dir    - true  shift left  pShift_val and insert input at position 2^0
 *                  - false shift right pShift_val and insert input at position 2^7
 * RETURN:          - 0|1 bit which was shifted out depending on direction left/right
 *                  - -1 on ERROR

 */
int Shift( uint16_t *pShift_val, unsigned shift_in, unsigned shift_dir ) {
    unsigned shift_reg;
    int shift_out;
    int mask;

    if( NULL == pShift_val ) {
        return -1;
    }

    shift_reg   = *pShift_val;  // get actual shift bits
    if( (SHIFT_DIR_LEFT == shift_dir ) ) {
        mask      = 1<<(MAX_LEDS-1);                // MSB highest bit will be shiftet out
        shift_out = (mask & shift_reg) ? 1 : 0;     // extract bit value
        shift_reg <<= 1;                            // left shift register value
        shift_reg |= shift_in ? 1 : 0 ;             // insert shift_in value at LSB position
    }
    else {  // (SHIFT_DIR_RIGHT == shift_dir )
        mask        = 1;                            // LSB lowest bit will be shiftet out
        shift_out   = (mask & shift_reg) ? 1 : 0 ;  // extract bit value
        shift_reg   >>= 1;                          // right shift register value
        shift_reg   |= shift_in ? 1<<(MAX_LEDS-1) : 0 ; // insert shift_in value at MSB position
    }
    *pShift_val = shift_reg & ( (1<<MAX_LEDS)-1 );  // write back shifted value
    return  shift_out;
}

// ====================================
void loop() {
    static uint32_t         shift_cnt   = 0;
    static unsigned long    prevMillis  = 0;
    static unsigned long    loop_cnt    = 0;

    unsigned long           currMillis = millis();
    unsigned long           diff       = currMillis - prevMillis;
    int shift_in;
    int shift_out;

    loop_cnt++;
    shift_in   = digitalRead( PIN_LED_IN );
    shift_cnt += shift_in ? 1 : 0;          // accumulate how often pin is high

    if( diff > timer) {
        prevMillis = currMillis;
        Serial.print("cnt=");
        Serial.print(loop_cnt);
        Serial.print("| ");
        // calculate if signal was long enough at  HIGH state
        shift_in = (shift_cnt>PIN_LED_IN_THRESHOLD) ? 1 : 0 ;

        shift_out   = Shift( &LEDS, shift_in, SHIFT_DIR );
        digitalWrite( PIN_LED_OUT, shift_out );
        update_LEDS( LEDS );
        Serial.print(" SO=");
        Serial.println(shift_out);


        shift_cnt = 0;      // reset debouncing counter
        loop_cnt  = 0;
    }
}
