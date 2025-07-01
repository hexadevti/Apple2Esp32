int joystickCycles0 = 0;
int joystickCycles1 = 0;
int joystickCycles2 = 0;
int joystickCycles3 = 0;

int analog_x = 0;
int analog_y = 0;
int digital_button1;


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
      pinMode(DIGITAL_BUTTON12_PIN, INPUT);
    
      xTaskCreate(analogJoystickTask, "analogJoystickTask", 4096, NULL, 1, NULL);
  }
}

void analogJoystickTask(void *pvParameters)
{
    while (running)
    {
        analog_x = analogRead(ANALOG_X_PIN);
        analog_y = analogRead(ANALOG_Y_PIN);
        digital_button1 = analogRead(DIGITAL_BUTTON12_PIN);

        timerpdl0 = (4095-analog_y) * 0.625;
        timerpdl1 = (4095-analog_x) * 0.625;
               if (digital_button1 > 3000 && digital_button1<= 4095) { // 0000
                Pb0 = false; Pb1 = false; Pb2 = false; Pb3 = false; 
        } else if (digital_button1 >  205 && digital_button1 <  215) { // 0001
            Pb0 = true; Pb1 = false; Pb2 = false; Pb3 = false;
        } else if (digital_button1 > 1890 && digital_button1 < 1900) { // 0010
            Pb0 = false; Pb1 = true; Pb2 = false; Pb3 = false;
        } else if (digital_button1 >  175 && digital_button1 <  185) { // 0011
            Pb0 = true; Pb1 = true; Pb2 = false; Pb3 = false;
        } else if (digital_button1 > 1505 && digital_button1 < 1515) { // 0100   
            Pb0 = false; Pb1 = false; Pb2 = true; Pb3 = false;
        } else if (digital_button1 >  165 && digital_button1 <  175) { // 0101
            Pb0 = true; Pb1 = false; Pb2 = true; Pb3 = false;
        } else if (digital_button1 > 1015 && digital_button1 < 1025) { // 0110
            Pb0 = false; Pb1 = true; Pb2 = true; Pb3 = false;
        } else if (digital_button1 >  140 && digital_button1 <  150) { // 0111
            Pb0 = true; Pb1 = true; Pb2 = true; Pb3 = false;
        } else if (digital_button1 >  570 && digital_button1 <  580) { // 1000
            Pb0 = false; Pb1 = false; Pb2 = false; Pb3 = true;
        } else if (digital_button1 >   95 && digital_button1 <  110) { // 1001
            Pb0 = true; Pb1 = false; Pb2 = false; Pb3 = true;
        } else if (digital_button1 >  455 && digital_button1 <  465) { // 1010
            Pb0 = false; Pb1 = true; Pb2 = false; Pb3 = true;
        } else if (digital_button1 >   81 && digital_button1 <   95) { // 1011
            Pb0 = true; Pb1 = true; Pb2 = false; Pb3 = true;
        } else if (digital_button1 >  415 && digital_button1 <  425) { // 1100
            Pb0 = false; Pb1 = false; Pb2 = true; Pb3 = true;
        } else if (digital_button1 >   70 && digital_button1 <   82) { // 1101
            Pb0 = true; Pb1 = false; Pb2 = true; Pb3 = true;
        } else if (digital_button1 >  335 && digital_button1 <  350) { // 1110
            Pb0 = false; Pb1 = true; Pb2 = true; Pb3 = true;
        } else if (digital_button1 >   60 && digital_button1 <   70) { // 1111
            Pb0 = true; Pb1 = true; Pb2 = true; Pb3 = true;
        }
        //Pb0 = !digital_button1;
        //Pb1 = !digital_button2;
        //Serial.printf("analog x=%d y=%d, btn1=%d\n", analog_x, analog_y, digital_button1);
        Serial.printf(" Pb0=%d, Pb1=%d Pb2=%d, Pb3=%d (%d)\n", Pb0, Pb1, Pb2, Pb3, digital_button1);

        // sprintf(buf, "timer %f %f", timerpdl0, timerpdl1);
        // Serial.println(buf);
        delay(100);
    }
    
}