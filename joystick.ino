int joystickCycles0 = 0;
int joystickCycles1 = 0;
int joystickCycles2 = 0;
int joystickCycles3 = 0;

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