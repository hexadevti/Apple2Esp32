void speakerSetup() {
  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);
}

void speakerToggle() {
  speaker_state = !speaker_state;
  digitalWrite(SPEAKER_PIN, speaker_state ? (sound ? HIGH : LOW) : LOW);
}