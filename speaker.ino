#define SPEAKER_PIN 26

const int freq = 5000;
const int resolution = 8;

boolean speaker_state = false;

void speaker_begin() {
  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);
  //ledcAttach(SPEAKER_PIN, freq, resolution);
}

void speaker_toggle() {
  speaker_state = !speaker_state;
  //REG_WRITE(speaker_state ? GPIO_OUT_W1TS_REG : GPIO_OUT_W1TC_REG, BIT4);
  digitalWrite(SPEAKER_PIN, speaker_state ? HIGH : LOW);
  //ledcWrite(SPEAKER_PIN, speaker_state ? 255 : 0);
  


}
