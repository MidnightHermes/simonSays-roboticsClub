// Pins used by the Arduino UNO
int BUZZER        = 6;
int RED_LED       = 3;
int RED_BUTTON    = 2;
int YELLOW_LED    = 9;
int YELLOW_BUTTON = 10;

// Frequencies played by the Buzzer (all in Hz)
int RED_FREQ    = 330;
int YELLOW_FREQ = 220;
int FAIL_FREQ   = 880;

// Constants used to define the gameplay flow
int BLINK_LEN = 300;      // The length of a single blink
int BLINK_LEN_MIN = 125;  // The length of a single blink
int BLINK_DELAY = 100;    // The delay between blinks
int FAIL_LEN = 1000;      // The length of the failure buzz
int TURN_DELAY = 1000;    // The length between turns
int INPUT_DELAY = 150;    // The time between inputs
// Misc variables used by game program
int seq[100];
int num_steps = 0;


/*
 * setup() is run once at startup
 */
void setup() {
  // Enable all the pins as either input (using onboard pullup resistor) or output
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_LED, OUTPUT);

  // Set the seed for random number generation using
  //   analogRead(0) to read some arbitrary voltage from pin 0
  //   to make sure the game is different every playthrough.
  randomSeed(analogRead(0));
}


/*
 * blink_buzz(time, pin) will simultaneously blink and buzz the
 *   corresponding LED and Frequency according to the specified pin
 *   for a specified length of time.
 */
void blink_buzz(int time, int pin) {
  // Set the frequency, led pin, and button pin to be blinked and buzzed
  //   according to the input pin (Yellow or Red).
  int frequency, led_pin, button_pin;
  if (pin == RED_LED) {
    frequency  = RED_FREQ;
    led_pin    = RED_LED;
    button_pin = RED_BUTTON;
  } else if (pin == YELLOW_LED) {
    frequency  = YELLOW_FREQ;
    led_pin    = YELLOW_LED;
    button_pin = YELLOW_BUTTON;
  }

  // tone(PIN, FREQUENCY) will turn PIN on and off,
  //   FREQUENCY times per second.
  tone(BUZZER, frequency);
  // digitalWrite(PIN, VALUE) will set PIN to VALUE;
  //   HIGH (0x1) is considered on, and LOW (0x0) is considered off.
  digitalWrite(led_pin, HIGH);

  // If we specified a time, we should delay that long;
  //   this is essentially how long the blink and buzz last.
  if (time > 0) delay(time);
  else {
    // If no time is specified, we just delay some minimum length of time.
    delay(BLINK_LEN_MIN);
    // Otherwise, while the button is still being pressed (LOW),
    //   spin infinitely until the user lets go.
    while (digitalRead(button_pin) == LOW);
  }

  // noTone(PIN) stops pulsing the pin at PIN
  noTone(BUZZER);
  //   and we set the led pin to LOW to turn it off.
  digitalWrite(led_pin, LOW);
}


/*
 * loop() is the function that runs infinitely
 *   while the Arduino UNO is powered on.
 *
 * When the end of loop() is reached, the Arduino UNO
 *   jumps back to the top of loop() and starts over.
 */
void loop() {
  // Append a random number to the current sequence
  seq[num_steps++] = random(0, 2);
  // Then play the sequence for the user
  for (int i = 0; i < num_steps; i++) {
    if (seq[i] == 0) blink_buzz(BLINK_LEN, YELLOW_LED);
    else blink_buzz(BLINK_LEN, RED_LED);
    // Between each blink/buzz, we wait BLINK_DELAY milliseconds
    //   before blinking/buzzing again.
    delay(BLINK_DELAY);
  }

  // Start at step 0,
  int step = 0;
  int player;
  //   and have the user go through each step, one at a time.
  while (step < num_steps) {
    // Wait for user input,
    int redState = HIGH;
    int yellowState = HIGH;
    while (yellowState == HIGH && redState == HIGH) {
      redState = digitalRead(RED_BUTTON);
      yellowState = digitalRead(YELLOW_BUTTON);
    }
    //   then blink/buzz the correct LED accordingly,
    if (redState == LOW) {
      blink_buzz(-1, RED_LED);
      player = 1;
    } else if (yellowState == LOW) {
      blink_buzz(-1, YELLOW_LED);
      player = 0;
    }
    //   waiting INPUT_DELAY microseconds between each input.
    delay(INPUT_DELAY);

    // If the player hit the wrong button, they fail,
    if (player != seq[step]) {
      tone(BUZZER, FAIL_FREQ);
      delay(FAIL_LEN);
      //   and the game loop ends and restarts.
      num_steps = 0;
      break;
    }

    step++;
  }

  // Wait TURN_DELAY microseconds between each turn
  delay(TURN_DELAY);
}
