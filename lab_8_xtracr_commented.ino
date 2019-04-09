// this is to test the changes in the github
//what if i added this comment

int leds[4] = { 24, 25, 26, 27 }; //set up LED pins
int tempo = 0;
int sequenceSwitch = 36;


int pushbuttons[4] = { 31, 32, 33, 34 };//push button pins
boolean lastChannelButtonState[2];
boolean channelButtonState[2];
int channelButtonPin[2] = {37, 29};
int nextChannelButtonPin = 37;
int prevChannelButtonPin = 29;
//set button pins to switch between channels
int channelDisplayed = 0;
//channel variable

int mappedMIDI[3][4] = {
  {59, 60, 67, 67},
  {60, 64, 67, 72},
  {67, 71, 74, 76}
};//different midi pattern for each channel

int currentstep = 0;//variable for the each note and LED
unsigned long lastSteptime = 0;//time generated variable for time checking

boolean lastButtonState[3][4] = {
  {LOW, LOW, LOW, LOW},
  {LOW, LOW, LOW, LOW},
  {LOW, LOW, LOW, LOW}
};

boolean buttonState[3][4] = {
  {LOW, LOW, LOW, LOW},
  {LOW, LOW, LOW, LOW},
  {LOW, LOW, LOW, LOW}
};
//button state multi-arrays
boolean on[3][4] = {
  { LOW, HIGH, LOW, HIGH },
  { LOW, HIGH, HIGH, HIGH },
  { LOW, LOW, LOW, HIGH }
};
//set arrays for button state check




void setup() {
  pinMode(pushbuttons[0], INPUT);
  pinMode(pushbuttons[1], INPUT);
  pinMode(pushbuttons[2], INPUT);
  pinMode(pushbuttons[3], INPUT);
  pinMode(sequenceSwitch, INPUT);
  pinMode(channelButtonPin[0], INPUT);
  pinMode(channelButtonPin[1], INPUT);
  pinMode(leds[0], OUTPUT);
  pinMode(leds[1], OUTPUT);
  pinMode(leds[2], OUTPUT);
  pinMode(leds[3], OUTPUT);

}

void loop() {
  tempo = analogRead(A16);
  checkChannelButton();//check channel first
  pushbuttonOnOff(channelDisplayed);//then check the button state for this channel
  sequence(channelDisplayed);//play the sequence based on the channel
}


void sequence(int channelDisplayed) {
  if (millis() > lastSteptime + tempo) {
    digitalWrite(leds[currentstep], LOW);
    midiPlayOff();
    if (digitalRead(sequenceSwitch) == HIGH) {
      currentstep = currentstep - 1;
      if (currentstep < 0) {
        currentstep = 3;
      }
    } else if (digitalRead(sequenceSwitch) == LOW) {
      currentstep += 1;
      if (currentstep > 3) {
        currentstep = 0;
      }
      //set the variable range to[0,3]
      //use the change of currentstep to control the direction of the sequencer
    }
    if (on[channelDisplayed][currentstep] == true) {
      digitalWrite(leds[currentstep], HIGH);
      midiPlayOn();
    } else if (on[channelDisplayed][currentstep] == false) {
      digitalWrite(leds[currentstep], LOW);
      midiPlayOff();
    }
    //change the status of LEDs and play midi notesaccording to the button state
    lastSteptime = millis();
    //set time check variable to change the tempo
  }
}

void midiPlayOn() {
  for (int i = 0; i < 3; i++) {
    usbMIDI.sendNoteOn(mappedMIDI[i][currentstep], 127, 1);
  }
}

void midiPlayOff() {
  for (int i = 0; i < 3; i++) {
    usbMIDI.sendNoteOn(mappedMIDI[i][currentstep], 0, 1);
  }
}

// these two functions control the midi playback

void checkChannelButton() {
  for (int i = 0; i < 2; i++) {
    lastChannelButtonState[i] = channelButtonState[i];
    channelButtonState[i] = digitalRead(channelButtonPin[i]);
  }
  if (channelButtonState[0] == LOW && lastChannelButtonState[0] == HIGH) {
    channelDisplayed += 1;
    if (channelDisplayed > 3) {
      channelDisplayed = 0;
    }
  } else if (channelButtonState[1] == LOW && lastChannelButtonState[1] == HIGH) {
    channelDisplayed -= 1;
    if (channelDisplayed < 0) {
      channelDisplayed = 2;
    }
  }
  Serial.println(channelDisplayed);//print the channel number every time when the channel changes
}


void pushbuttonOnOff(int channelDisplayed) {
  for (int i = 0; i < 4; i++) {
    lastButtonState[channelDisplayed][i] = buttonState[channelDisplayed][i];
    buttonState[channelDisplayed][i] = digitalRead(pushbuttons[i]);
    if (buttonState[channelDisplayed][i] == HIGH && lastButtonState[channelDisplayed][i] == LOW) {
      on[channelDisplayed][i] = !on[channelDisplayed][i];
      //change the state of on[]if any button is pressed
    }
  }
}
//this is a button state check function
