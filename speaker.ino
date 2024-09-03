#define SPEAKER_PIN 4

boolean speaker_state = false;

void speaker_begin() {
  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);
}

void speaker_toggle() {
  speaker_state = !speaker_state;
  digitalWrite(SPEAKER_PIN, speaker_state);
}
