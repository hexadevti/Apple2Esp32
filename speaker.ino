void speakerSetup() {
  if (!dacSound) {
    pinMode(SPEAKER_PIN, OUTPUT);
    digitalWrite(SPEAKER_PIN, LOW);
  }
}

void speakerToggle() {
  speaker_state = !speaker_state;
  // if (dacSound)
  //   dacWrite(SPEAKER_PIN, speaker_state ? volume : 0);
  // else
    digitalWrite(SPEAKER_PIN, speaker_state ? HIGH : LOW);
}