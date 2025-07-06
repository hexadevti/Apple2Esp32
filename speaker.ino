void speakerSetup() {
  #ifdef DAC
    if (!dacSound) {
      digitalWrite(SPEAKER_PIN, LOW);
      pinMode(SPEAKER_PIN, OUTPUT);
    }
  #else
    digitalWrite(SPEAKER_PIN, LOW);
    pinMode(SPEAKER_PIN, OUTPUT);
  #endif
    
}

void speakerToggle() {
  speaker_state = !speaker_state;
  #ifdef DAC
    if (dacSound)
      dacWrite(SPEAKER_PIN, speaker_state ? volume : 0);
    else
  #endif
    digitalWrite(SPEAKER_PIN, speaker_state ? HIGH : LOW);
}