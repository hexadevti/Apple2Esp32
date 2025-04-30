void speakerSetup() {
  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);
}

void speakerToggle() {
  speaker_state = !speaker_state;
  //REG_WRITE(speaker_state ? GPIO_OUT_W1TS_REG : GPIO_OUT_W1TC_REG, BIT4);
  digitalWrite(SPEAKER_PIN, speaker_state ? HIGH : LOW);
}