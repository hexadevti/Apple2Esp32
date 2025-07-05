void speakerSetup() {
  if (dacSound) {
    digitalWrite(SPEAKER_PIN, LOW);
    pinMode(SPEAKER_PIN, OUTPUT);
  }
}

void speakerToggle() {
  speaker_state = !speaker_state;
  if (dacSound)
    dacWrite(SPEAKER_PIN, speaker_state ? volume : 0);
  else
    digitalWrite(SPEAKER_PIN, speaker_state ? HIGH : LOW);
}