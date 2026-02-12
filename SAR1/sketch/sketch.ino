#include <LiquidCrystal.h>     //the liquid crystal library contains commands for printing to the display

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);   // tell the RedBoard what pins are connected to the display
int switchPin = 2;    // pin to read the switch changing states
int count = 0;        // tracks the times the switch changes
bool lastSwitchState; // Tracks the last position of the switch

void setup() {
  pinMode(switchPin, INPUT_PULLUP);   // since the pin is a variable we can change it easier
  lcd.begin(16, 2);   // initialize screen
  lcd.clear();     // clear anything leftover
  
  lcd.setCursor(0, 0);
  lcd.print("Nap time!");

  // Initialize the starting state of the switch
  lastSwitchState = digitalRead(switchPin);
}

void loop() {
  // --- Clock Logic (Always Runs) ---
  long totalSeconds = millis() / 1000;  //time conversions for "normal clock"
  int seconds = totalSeconds % 60;
  int minutes = (totalSeconds / 60) % 60;
  int hours = totalSeconds / 3600;

  // --- Clock Display Layout ---
  lcd.setCursor(0, 1);
  if (hours < 10) lcd.print("0");
  lcd.print(hours);
  lcd.print(":");
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);

  // --- Slide Switch Logic ---
  bool currentSwitchState = digitalRead(switchPin);  // bool for 1/0 logic checks

  lcd.setCursor(11, 0);   //label for count
  lcd.print("Count");

  // Check if the switch position has changed at all
  if (currentSwitchState != lastSwitchState) {
    count++;                       // Increment the count
    
    lcd.setCursor(12, 1);
    //lcd.print("   ");              // Clear old digits, doesn't seem necessary
    lcd.setCursor(12, 1);
    lcd.print(count);
    
    lastSwitchState = currentSwitchState; // Save the new position
    delay(50); // Debounce to prevent flickering during the slide
  }
}