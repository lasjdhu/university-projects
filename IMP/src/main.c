/*
 * @file main.c
 * @author Dmitrii Ivanushkin (xivanu00)
 * @brief Simple alarm in FITkit 3 implementation
 */
#include "MK60D10.h"

// Traffic lights LED and Piezo
#define SEM_RED (1 << 9)
#define SEM_YELLOW (1 << 8)
#define SEM_GREEN (1 << 7)
#define PIEZO (1 << 9)

// Motion detector, Reed, Tilt
#define SENSOR_PIR (1 << 25)
#define SENSOR_REED (1 << 8)
#define SENSOR_TILT (1 << 6)

// Keypad Matrix
#define ROW1_PIN 10
#define ROW2_PIN 11
#define ROW3_PIN 28
#define ROW4_PIN 27
#define COL1_PIN 29
#define COL2_PIN 28
#define COL3_PIN 24
#define COL4_PIN 26

// RFID RC522 Module
#define RFID_SCK_PIN 12
#define RFID_MOSI_PIN 13
#define RFID_MISO_PIN 14
#define RFID_SDA_PIN 15

#define RFID_SDA_LO (PTD->PDOR &= ~(1 << RFID_SDA_PIN))
#define RFID_SDA_HI (PTD->PDOR |= (1 << RFID_SDA_PIN))
#define RFID_SCK_LO (PTD->PDOR &= ~(1 << RFID_SCK_PIN))
#define RFID_SCK_HI (PTD->PDOR |= (1 << RFID_SCK_PIN))
#define RFID_MOSI_LO (PTD->PDOR &= ~(1 << RFID_MOSI_PIN))
#define RFID_MOSI_HI (PTD->PDOR |= (1 << RFID_MOSI_PIN))
#define RFID_MISO_READ ((PTD->PDIR >> RFID_MISO_PIN) & 0x01)

typedef enum {
  STATE_IDLE,       // System disarmed, waiting for input
  STATE_EXIT_DELAY, // User authenticated, countdown to arming
  STATE_ARMED,      // System active, monitoring sensors
  STATE_ALARM       // Intrusion detected, siren active
} SystemState_t;

SystemState_t currentState = STATE_IDLE;

char passBuffer[10]; // Keypad input
int passIdx = 0;
char selectedMode = 'D'; // Security mode
unsigned int alarmTimer = 0;
volatile unsigned long tick = 0;
int blinkCounter = 0;
int rfidCooldown =
    0; // Prevents multiple reads of the same card in a short period  of time

void delay(long long bound) {
  for (long long i = 0; i < bound; i++)
    __asm("nop");
}

void beep_short(void) {
  for (int k = 0; k < 100; k++) {
    GPIOA_PDOR ^= PIEZO;
    delay(400);
  }
  GPIOA_PDOR &= ~PIEZO;
}

void beep_long(void) {
  for (int k = 0; k < 500; k++) {
    GPIOA_PDOR ^= PIEZO;
    delay(400);
  }
  GPIOA_PDOR &= ~PIEZO;
}

void play_alarm_tone(void) {
  for (int k = 0; k < 200; k++) {
    GPIOA_PDOR ^= PIEZO;
    delay(400);
  }
  GPIOA_PDOR &= ~PIEZO;
}

void leds_off_all(void) {
  PTD->PDOR &= ~(SEM_RED | SEM_YELLOW);
  PTA->PDOR &= ~SEM_GREEN;
}

void led_green_on(void) { PTA->PDOR |= SEM_GREEN; }
void led_yellow_on(void) { PTD->PDOR |= SEM_YELLOW; }
void led_yellow_toggle(void) { PTD->PDOR ^= SEM_YELLOW; }
void led_red_on(void) { PTD->PDOR |= SEM_RED; }

void blink_error(void) {
  leds_off_all();
  for (int i = 0; i < 2; i++) {
    PTD->PDOR |= SEM_RED;
    delay(2000000);
    PTD->PDOR &= ~SEM_RED;
    delay(2000000);
  }
}

void SPI_Init(void) {
  RFID_SDA_HI; // Deselect
  RFID_SCK_LO; // Idle clock
}

unsigned char SPI_Transfer(unsigned char data) {
  unsigned char rx = 0;
  for (int i = 0; i < 8; i++) {
    // Write MOSI bit
    if (data & 0x80)
      RFID_MOSI_HI;
    else
      RFID_MOSI_LO;
    data <<= 1;

    // Pulse clock
    RFID_SCK_HI;

    // Read MISO bit
    rx <<= 1;
    if (RFID_MISO_READ)
      rx |= 1;

    // Pulse clock
    RFID_SCK_LO;
  }
  return rx;
}

void RC522_WriteReg(unsigned char addr, unsigned char val) {
  RFID_SDA_LO;                      // Select
  SPI_Transfer((addr << 1) & 0x7E); // Send Address
  SPI_Transfer(val);                // Send Data
  RFID_SDA_HI;                      // Release
}

unsigned char RC522_ReadReg(unsigned char addr) {
  unsigned char val;
  RFID_SDA_LO;
  SPI_Transfer(((addr << 1) & 0x7E) | 0x80); // Send Address
  val = SPI_Transfer(0x00);                  // Read Data
  RFID_SDA_HI;
  return val;
}

void RC522_Init(void) {
  RC522_WriteReg(0x01, 0x0F); // Software reset (hardware is not connected)
  delay(50000);
  RC522_WriteReg(0x2A, 0x8D);
  RC522_WriteReg(0x2B, 0x3E);
  RC522_WriteReg(0x2D, 30);
  RC522_WriteReg(0x2C, 0);
  RC522_WriteReg(0x15, 0x40);
  RC522_WriteReg(0x11, 0x3D);
  unsigned char temp = RC522_ReadReg(0x14);
  if (!(temp & 0x03))
    RC522_WriteReg(0x14, temp | 0x03);
}

int RC522_CheckCard_Standard(void) {
  unsigned char n;
  unsigned int k;

  // Prepare command
  RC522_WriteReg(0x04, 0x7F);
  RC522_WriteReg(0x0A, 0x80);
  RC522_WriteReg(0x02, 0x77);
  RC522_WriteReg(0x0D, 0x07);
  RC522_WriteReg(0x09, 0x26);
  RC522_WriteReg(0x01, 0x0C);
  RC522_WriteReg(0x0D, 0x87);

  // Wait for response
  for (k = 0; k < 2000; k++) {
    n = RC522_ReadReg(0x04);
    if (n & 0x30)
      break;
  }

  RC522_WriteReg(0x0D, 0x07);
  if (k < 2000) {
    if (!(RC522_ReadReg(0x06) & 0x1B))
      return 1; // Success
  }
  return 0;
}

void MCUInit(void) {
  // Config clock and disable watchdog
  MCG_C4 |= (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01));
  SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
  WDOG_UNLOCK = 0xC520;
  WDOG_UNLOCK = 0xD928;
  WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

void PortsInit(void) {
  // Enable clock
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK |
                SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

  PORTA->PCR[25] = PORT_PCR_MUX(0x01); // PIR
  PORTA->PCR[8] =
      PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; // Reed
  PORTA->PCR[6] =
      PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; // Tilt

  PORTA->PCR[7] = PORT_PCR_MUX(0x01); // Green
  PORTA->PCR[9] = PORT_PCR_MUX(0x01); // Red
  PORTD->PCR[9] = PORT_PCR_MUX(0x01); // Piezo
  PORTD->PCR[8] = PORT_PCR_MUX(0x01); // Yellow

  // Keypad
  PORTA->PCR[ROW1_PIN] = PORT_PCR_MUX(0x01);
  PORTA->PCR[ROW2_PIN] = PORT_PCR_MUX(0x01);
  PORTA->PCR[ROW4_PIN] = PORT_PCR_MUX(0x01);
  PORTE->PCR[ROW3_PIN] = PORT_PCR_MUX(0x01);
  PORTA->PCR[COL1_PIN] =
      PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
  PORTA->PCR[COL2_PIN] =
      PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
  PORTA->PCR[COL3_PIN] =
      PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
  PORTA->PCR[COL4_PIN] =
      PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

  // RFID
  PORTD->PCR[RFID_SCK_PIN] = PORT_PCR_MUX(0x01);
  PORTD->PCR[RFID_MOSI_PIN] = PORT_PCR_MUX(0x01);
  PORTD->PCR[RFID_MISO_PIN] = PORT_PCR_MUX(0x01);
  PORTD->PCR[RFID_SDA_PIN] = PORT_PCR_MUX(0x01);

  // Outputs
  PTA->PDDR |=
      (SEM_GREEN | PIEZO | (1 << ROW1_PIN) | (1 << ROW2_PIN) | (1 << ROW4_PIN));
  PTD->PDDR |= (SEM_RED | SEM_YELLOW | (1 << RFID_SCK_PIN) |
                (1 << RFID_MOSI_PIN) | (1 << RFID_SDA_PIN));
  PTE->PDDR |= (1 << ROW3_PIN);

  // Inputs
  PTA->PDDR &= ~(SENSOR_PIR | SENSOR_REED | SENSOR_TILT);
  PTD->PDDR &= ~((1 << RFID_MISO_PIN));
}

char Keypad_Scan_Raw(void) {
  char keys[4][4] = {{'1', '2', '3', 'A'},
                     {'4', '5', '6', 'B'},
                     {'7', '8', '9', 'C'},
                     {'*', '0', '#', 'D'}};
  for (int r = 0; r < 4; r++) {
    PTA->PDOR |= (1 << ROW1_PIN) | (1 << ROW2_PIN) | (1 << ROW4_PIN);
    PTE->PDOR |= (1 << ROW3_PIN);

    if (r == 0)
      PTA->PDOR &= ~(1 << ROW1_PIN);
    if (r == 1)
      PTA->PDOR &= ~(1 << ROW2_PIN);
    if (r == 2)
      PTE->PDOR &= ~(1 << ROW3_PIN);
    if (r == 3)
      PTA->PDOR &= ~(1 << ROW4_PIN);

    delay(30000);

    if (!(PTA->PDIR & (1 << COL1_PIN)))
      return keys[r][0];
    if (!(PTA->PDIR & (1 << COL2_PIN)))
      return keys[r][1];
    if (!(PTA->PDIR & (1 << COL3_PIN)))
      return keys[r][2];
    if (!(PTA->PDIR & (1 << COL4_PIN)))
      return keys[r][3];
  }
  return 0;
}

char Keypad_Get_Single_Press(void) {
  static char lastKey = 0;
  char currentKey = Keypad_Scan_Raw();
  if (currentKey != 0 && currentKey != lastKey) {
    lastKey = currentKey;
    return currentKey;
  }
  if (currentKey == 0)
    lastKey = 0;
  return 0;
}

int main(void) {
  // Hardware init
  MCUInit();
  PortsInit();
  SPI_Init();
  RC522_Init();

  char key;
  static int exitTimer = 0;

  // Reset
  leds_off_all();
  led_green_on();

  while (1) {
    tick++;
    key = Keypad_Get_Single_Press();

    // RFID cooldown management
    if (rfidCooldown > 0) {
      rfidCooldown--;
    }

    if (key != 0) {
      beep_short();

      // Reset buffer on '*'
      if (key == '*')
        passIdx = 0;

      // Store key in buffer
      if (passIdx < 9) {
        passBuffer[passIdx++] = key;
        passBuffer[passIdx] = '\0';
      }

      // Format: *<MODE>1234
      if (currentState == STATE_IDLE) {
        if (passIdx == 6) {
          if (passBuffer[0] == '*' &&
              (passBuffer[1] >= 'A' && passBuffer[1] <= 'D') &&
              passBuffer[2] == '1' && passBuffer[3] == '2' &&
              passBuffer[4] == '3' && passBuffer[5] == '4') {

            selectedMode = passBuffer[1];
            beep_long();
            passIdx = 0;
            exitTimer = 0;
            blinkCounter = 0;
            leds_off_all();
            currentState = STATE_EXIT_DELAY;
          } else {
            blink_error(); // Wrong password
            passIdx = 0;
            leds_off_all();
            led_green_on();
          }
        }
      }
      // Format: *1234
      else if (currentState == STATE_ARMED || currentState == STATE_ALARM ||
               currentState == STATE_EXIT_DELAY) {
        if (passIdx == 5) {
          if (passBuffer[0] == '*' && passBuffer[1] == '1' &&
              passBuffer[2] == '2' && passBuffer[3] == '3' &&
              passBuffer[4] == '4') {

            beep_long();
            beep_long();
            passIdx = 0;
            GPIOA_PDOR &= ~PIEZO;
            currentState = STATE_IDLE;
            leds_off_all();
            led_green_on();
          } else {
            blink_error(); // Wrong password
            passIdx = 0;
            leds_off_all();
            // Restore previous state
            if (currentState == STATE_ARMED)
              led_yellow_on();
            if (currentState == STATE_ALARM)
              led_red_on();
          }
        }
      }
    }

    switch (currentState) {
    case STATE_IDLE:
      // Check RFID to arm
      if (rfidCooldown == 0 && RC522_CheckCard_Standard()) {
        rfidCooldown = 60;
        beep_short();
        exitTimer = 0;
        blinkCounter = 0;
        leds_off_all();
        currentState = STATE_EXIT_DELAY;
      }
      break;

    case STATE_EXIT_DELAY:
      // Give time to leave before arming
      exitTimer++;
      if (exitTimer >= 10) {
        led_yellow_toggle();
        exitTimer = 0;
        blinkCounter++;
      }
      // After 10 blinks arm system
      if (blinkCounter >= 10) {
        blinkCounter = 0;
        exitTimer = 0;
        beep_long();
        currentState = STATE_ARMED;
      }
      break;

    case STATE_ARMED:
      led_yellow_on();
      PTA->PDOR &= ~SEM_GREEN;

      // Check RFID to disarm
      if (rfidCooldown == 0 && RC522_CheckCard_Standard()) {
        rfidCooldown = 60;
        beep_long();
        currentState = STATE_IDLE;
        leds_off_all();
        led_green_on();
      }

      // Different sensors enabled based on mode (all in D mode)
      int triggered = 0;
      if ((selectedMode == 'A' || selectedMode == 'D') &&
          (PTA->PDIR & SENSOR_PIR))
        triggered = 1;
      if ((selectedMode == 'B' || selectedMode == 'D') &&
          (PTA->PDIR & SENSOR_REED))
        triggered = 1;
      if ((selectedMode == 'C' || selectedMode == 'D') &&
          (PTA->PDIR & SENSOR_TILT))
        triggered = 1;

      if (triggered)
        currentState = STATE_ALARM;
      break;

    case STATE_ALARM:
      led_red_on();
      PTD->PDOR &= ~SEM_YELLOW;

      // Siren
      alarmTimer++;
      if (alarmTimer > 4) {
        play_alarm_tone();
        alarmTimer = 0;
      }

      // Check RFID to disarm
      if (rfidCooldown == 0 && RC522_CheckCard_Standard()) {
        rfidCooldown = 60;
        beep_long();
        GPIOA_PDOR &= ~PIEZO;
        currentState = STATE_IDLE;
        leds_off_all();
        led_green_on();
      }
      break;
    }
  }
  return 0;
}
