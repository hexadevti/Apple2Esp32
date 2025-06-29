int joystickCycles0 = 0;
int joystickCycles1 = 0;
int joystickCycles2 = 0;
int joystickCycles3 = 0;

int analog_x = 0;
int analog_y = 0;
int digital_button1;
int digital_button2;

void processJoystick(float speedAdjust) {
    if (joystick) {
        if (CgReset0) {
            joystickCycles0++;
        }
        if (CgReset1) {
            joystickCycles1++;
        }
        if (CgReset2) {
            joystickCycles2++;
        }
        if (CgReset3) {
            joystickCycles3++;
        }

        if (joystickCycles0 >= static_cast<int>(timerpdl0 * speedAdjust)) {
            joystickCycles0 = 0;
            Cg0 = false;
            CgReset0 = false;
        }

        if (joystickCycles1 >= static_cast<int>(timerpdl1 * speedAdjust)) {
            joystickCycles1 = 0;
            Cg1 = false;
            CgReset1 = false;
        }

        if (joystickCycles2 >= static_cast<int>(timerpdl2 * speedAdjust)) {
            joystickCycles2 = 0;
            Cg2 = false;
            CgReset2 = false;
        }

        if (joystickCycles3 >= static_cast<int>(timerpdl3 * speedAdjust)) {
            joystickCycles3 = 0;
            Cg3 = false;
            CgReset3 = false;
        }
    }
}

void joystickSetup(bool analog)
{
    if (joystick)
    {
      timerpdl0 = JOY_MID;
      timerpdl1 = JOY_MID;
    }
    else
    {
      timerpdl0 = JOY_MAX;
      timerpdl1 = JOY_MAX;
    }

  if (analog) {
      pinMode(ANALOG_X_PIN, INPUT);
      pinMode(ANALOG_Y_PIN, INPUT);
      pinMode(DIGITAL_BUTTON1_PIN, INPUT);
      pinMode(DIGITAL_BUTTON2_PIN, INPUT);
    
      xTaskCreate(analogJoystickTask, "analogJoystickTask", 4096, NULL, 1, NULL);
  }
}

void analogJoystickTask(void *pvParameters)
{
    while (running)
    {
        analog_x = analogRead(ANALOG_X_PIN);
        analog_y = analogRead(ANALOG_Y_PIN);
        digital_button1 = analogRead(DIGITAL_BUTTON1_PIN);
        digital_button2 = digitalRead(DIGITAL_BUTTON2_PIN);

        timerpdl0 = (4095-analog_y) * 0.625;
        timerpdl1 = (4095-analog_x) * 0.625;
        if (digital_button1 > 3000) {
            Pb0 = false;
            Pb1 = false;
        } else if (digital_button1 > 1800 && digital_button1 < 2000) {
            Pb0 = true;
            Pb1 = false;    
        } else if (digital_button1 > 200 && digital_button1 < 300) {
            Pb0 = false;
            Pb1 = true;    
        } else {
            Pb0 = true;
            Pb1 = true;    
        }
        //Pb0 = !digital_button1;
        //Pb1 = !digital_button2;
        // sprintf(buf, "analog x=%d y=%d, btn1=%d, btn2=%d", analog_x, analog_y, digital_button1, digital_button2);
        // Serial.println(buf);
        // sprintf(buf, "timer %f %f", timerpdl0, timerpdl1);
        // Serial.println(buf);
        delay(100);
    }
    
}