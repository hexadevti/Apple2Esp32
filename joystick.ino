void processJoystick(float speedAdjust)
{
    if (joystick)
    {
        if (CgReset0)
        {
            joystickCycles0++;
        }
        if (CgReset1)
        {
            joystickCycles1++;
        }
        if (CgReset2)
        {
            joystickCycles2++;
        }
        if (CgReset3)
        {
            joystickCycles3++;
        }

        if (joystickCycles0 >= static_cast<int>(timerpdl0 * speedAdjust))
        {
            joystickCycles0 = 0;
            Cg0 = false;
            CgReset0 = false;
        }

        if (joystickCycles1 >= static_cast<int>(timerpdl1 * speedAdjust))
        {
            joystickCycles1 = 0;
            Cg1 = false;
            CgReset1 = false;
        }

        if (joystickCycles2 >= static_cast<int>(timerpdl2 * speedAdjust))
        {
            joystickCycles2 = 0;
            Cg2 = false;
            CgReset2 = false;
        }

        if (joystickCycles3 >= static_cast<int>(timerpdl3 * speedAdjust))
        {
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

    if (analog)
    {
        pinMode(ANALOG_X_PIN, INPUT);
        pinMode(ANALOG_Y_PIN, INPUT);
        pinMode(DIGITAL_BUTTON12_PIN, INPUT);
        pPb0 = Pb0;
        pPb1 = Pb1;
        pPb2 = Pb2;
        pPb3 = Pb3;
        xTaskCreate(analogJoystickTask, "analogJoystickTask", 4096, NULL, 1, NULL);
    }
}

static void buttonDown(uint8_t btn)
{
    if (btn == 3)
    {
        showHideOptionsWindow();
    }
    if (OptionsWindow)
    {
        if (btn == 0)
        {
            switch (fnSelected)
            {
            case 1:
                HdDisk = !HdDisk;
                break;
            case 2:
                AppleIIe = !AppleIIe;
                break;
            case 3:
                Fast1MhzSpeed = !Fast1MhzSpeed;
                break;
            case 4:
                sound = !sound;
                break;
            case 5:
                joystick = !joystick;
                break;
            case 6:
                videoColor = !videoColor;
                break;
            case 7:
                dacSound = !dacSound;
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
            }
            printOptionsBackground();
        }
        else if (btn == 1)
        {
            if (HdDisk == initializedHdDisk)
            {
                if (HdDisk)
                {
                    setHdFile();
                    saveHdFile();
                }
                else
                {
                    setDiskFile();
                    saveDiskFile();
                }
            }
            else
            {
                saveEEPROM();
                EEPROM.commit();
            }
            ESP.restart();
        }
    }
    else
    {
        if (!joystick)
        {
            if (btn == 0)
            {
                keymem = 0xa0;
            }
            else if (btn == 1)
            {
                keymem = 0x8d;
            }
            else if (btn == 2)
            {
                keymem = 0x9b;
            }
        }
    }
}

static void buttonUp(uint8_t btn)
{
    if (OptionsWindow)
    {
        if (fnSelected == 0)
        {
            if (btn == 0)
            {
                if (HdDisk)
                    setHdFile();
                else
                    setDiskFile();
                diskChanged = true;
                showHideOptionsWindow();
            }
        }
    }
}

static void changeDirection(bool x, uint8_t dir)
{
    if (OptionsWindow)
    {

        if (x)
        {
            if (dir == 0)
            {
                if (fnSelected != 0)
                {
                    fnSelected--;
                }
            }
            else if (dir == 2)
            {
                if (fnSelected < 7)
                {
                    fnSelected++;
                }
            }
            printOptionsBackground();
        }
        else
        { // Y
            if (dir == 0 || dir == 2)
            {
                if (fnSelected == 0)
                {
                    if (dir == 0)
                    { // Up
                        if (!HdDisk)
                            prevDiskFile();
                        else
                            prevHdFile();
                        listFiles(false);
                    }
                    else if (dir == 2)
                    { // down
                        if (!HdDisk)
                            nextDiskFile();
                        else
                            nextHdFile();
                        listFiles(true);
                    }
                }
            }
        }
    }
    else if (!joystick)
    {
        if (x)
        {
            if (dir == 0)
            {
                keymem = 0x88; // back key
            }
            else if (dir == 2)
            {
                keymem = 0x95; // forward key
            }
        }
        else
        {
            if (dir == 0)
            {
                keymem = 0x8b; // up
            }
            else if (dir == 2)
            {
                keymem = 0x8a; // down key
            }
        }
    }
}

static void analogJoystickTask(void *pvParameters)
{
    while (running)
    {
        analoxX = analogRead(ANALOG_X_PIN);
        analogY = analogRead(ANALOG_Y_PIN);
        digital_button1 = analogRead(DIGITAL_BUTTON12_PIN);

        timerpdl0 = (4095 - analoxX) * 0.625;
        timerpdl1 = (4095 - analogY) * 0.625;
        if (digital_button1 > 3000 && digital_button1 <= 4095)
        { // 0000
            Pb0 = false;
            Pb1 = false;
            Pb2 = false;
            Pb3 = false;
        }
        else if (digital_button1 > 205 && digital_button1 < 215)
        { // 0001
            Pb0 = true;
            Pb1 = false;
            Pb2 = false;
            Pb3 = false;
        }
        else if (digital_button1 > 1890 && digital_button1 < 1900)
        { // 0010
            Pb0 = false;
            Pb1 = true;
            Pb2 = false;
            Pb3 = false;
        }
        else if (digital_button1 > 175 && digital_button1 < 185)
        { // 0011
            Pb0 = true;
            Pb1 = true;
            Pb2 = false;
            Pb3 = false;
        }
        else if (digital_button1 > 1505 && digital_button1 < 1515)
        { // 0100
            Pb0 = false;
            Pb1 = false;
            Pb2 = true;
            Pb3 = false;
        }
        else if (digital_button1 > 165 && digital_button1 < 175)
        { // 0101
            Pb0 = true;
            Pb1 = false;
            Pb2 = true;
            Pb3 = false;
        }
        else if (digital_button1 > 1015 && digital_button1 < 1025)
        { // 0110
            Pb0 = false;
            Pb1 = true;
            Pb2 = true;
            Pb3 = false;
        }
        else if (digital_button1 > 140 && digital_button1 < 150)
        { // 0111
            Pb0 = true;
            Pb1 = true;
            Pb2 = true;
            Pb3 = false;
        }
        else if (digital_button1 > 570 && digital_button1 < 580)
        { // 1000
            Pb0 = false;
            Pb1 = false;
            Pb2 = false;
            Pb3 = true;
        }
        else if (digital_button1 > 95 && digital_button1 < 110)
        { // 1001
            Pb0 = true;
            Pb1 = false;
            Pb2 = false;
            Pb3 = true;
        }
        else if (digital_button1 > 455 && digital_button1 < 465)
        { // 1010
            Pb0 = false;
            Pb1 = true;
            Pb2 = false;
            Pb3 = true;
        }
        else if (digital_button1 > 81 && digital_button1 < 95)
        { // 1011
            Pb0 = true;
            Pb1 = true;
            Pb2 = false;
            Pb3 = true;
        }
        else if (digital_button1 > 415 && digital_button1 < 425)
        { // 1100
            Pb0 = false;
            Pb1 = false;
            Pb2 = true;
            Pb3 = true;
        }
        else if (digital_button1 > 70 && digital_button1 < 82)
        { // 1101
            Pb0 = true;
            Pb1 = false;
            Pb2 = true;
            Pb3 = true;
        }
        else if (digital_button1 > 335 && digital_button1 < 350)
        { // 1110
            Pb0 = false;
            Pb1 = true;
            Pb2 = true;
            Pb3 = true;
        }
        else if (digital_button1 > 60 && digital_button1 < 70)
        { // 1111
            Pb0 = true;
            Pb1 = true;
            Pb2 = true;
            Pb3 = true;
        }

        if (pPb0 != Pb0)
            if (Pb0)
                buttonDown(0);
            else
                buttonUp(0);
        if (pPb1 != Pb1)
            if (Pb1)
                buttonDown(1);
            else
                buttonUp(1);
        if (pPb2 != Pb2)
            if (Pb2)
                buttonDown(2);
            else
                buttonUp(2);
        if (pPb3 != Pb3)
            if (Pb3)
                buttonDown(3);
            else
                buttonUp(3);

        pPb0 = Pb0;
        pPb1 = Pb1;
        pPb2 = Pb2;
        pPb3 = Pb3;

        if (analogY >= 4095)
            joyX = 0; // Up
        else if (analogY > 0 && analogY < 4095)
            joyX = 1; // Center
        else
            joyX = 2; // Down

        if (analoxX >= 4095)
            joyY = 0; // Up
        else if (analoxX > 0 && analoxX < 4095)
            joyY = 1; // Center
        else
            joyY = 2; // Down

        if (pJoyX != joyX)
            changeDirection(0, joyX);
        if (pJoyY != joyY)
            changeDirection(1, joyY);

        pJoyX = joyX;
        pJoyY = joyY;

        // Pb0 = !digital_button1;
        // Pb1 = !digital_button2;
        Serial.printf("analog x=%d y=%d, btn1=%d\n", analoxX, analogY, digital_button1);
        // Serial.printf(" Pb0=%d, Pb1=%d Pb2=%d, Pb3=%d (%d)\n", Pb0, Pb1, Pb2, Pb3, digital_button1);

        // sprintf(buf, "timer %f %f", timerpdl0, timerpdl1);
        // Serial.println(buf);
        delay(100);
    }
}