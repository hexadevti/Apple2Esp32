int joystickCycles0 = 0;
int joystickCycles1 = 0;
int joystickCycles2 = 0;
int joystickCycles3 = 0;

int analog_x = 0;
int analog_y = 0;
int analog_x_pin = 35;
int analog_y_pin = 22;

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

void analogJoystickSetup()
{
    pinMode(analog_x_pin, INPUT);
    pinMode(analog_y_pin, INPUT);
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
  xTaskCreate(analogJoystickTask, "analogJoystickTask", 4096, NULL, 1, NULL);
}

void analogJoystickTask(void *pvParameters)
{
    while (running)
    {
        analog_x = analogReadMilliVolts(analog_x_pin);
        analog_y = analogReadMilliVolts(analog_y_pin);
        timerpdl0 = analog_x * 0.625;
        timerpdl1 = analog_y * 0.625;
        sprintf(buf, "analog %d %d", analog_x, analog_y);
        Serial.println(buf);
        // sprintf(buf, "timer %f %f", timerpdl0, timerpdl1);
        // Serial.println(buf);
        delay(100);
    }
    
}