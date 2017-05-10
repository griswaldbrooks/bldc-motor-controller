/* BLDC Motor Controller
 */

const unsigned int PHASE_A(2);
const unsigned int PHASE_B(3);
const unsigned int PHASE_C(4);
const unsigned int MODE_PIN(7);

void setup() 
{
    pinMode(PHASE_A, OUTPUT);
    pinMode(PHASE_B, OUTPUT);
    pinMode(PHASE_C, OUTPUT);
    pinMode(MODE_PIN, INPUT);
    Serial.begin(115200);
}

const unsigned int PHASE_SET[6][3] = {
    {1, 1, 0},
    {1, 0, 0},
    {1, 0, 1},
    {0, 0, 1},
    {0, 1, 1},
    {0, 1, 0}
};

void cycle(int delay_ms)
{
    static int ndx(0);
    if (delay_ms > 0)
    {
        ndx++;
        if (ndx >= 6)
        {
            ndx = 0;
        }
    }
    else if (delay_ms < 0)
    {
        ndx--;
        if (ndx < 0)
        {
            ndx = 5;
        }
    }
    else
    {
        digitalWrite(PHASE_A, 0);
        digitalWrite(PHASE_B, 0);
        digitalWrite(PHASE_C, 0);
        return;
    }
    
    Serial.print("PHASE_SET ndx = ");
    Serial.println(ndx);
    
    digitalWrite(PHASE_A, PHASE_SET[ndx][0]);
    digitalWrite(PHASE_B, PHASE_SET[ndx][1]);
    digitalWrite(PHASE_C, PHASE_SET[ndx][2]);
    
    delay(abs(delay_ms));
}

void loop()
{
    static int analog_value_prev(0);
    int analog_value_current = analogRead(0);
    int da = analog_value_current - analog_value_prev;
    Serial.print("analog_value_prev = ");
    Serial.println(analog_value_prev);
    Serial.print("analog_value_current = ");
    Serial.println(analog_value_current);
    Serial.print("da = ");
    Serial.println(da);

    int delay_ms(50.f * sin(millis() / 1000.f));
   
    // If mode is HIGH, do position control, else speed demo. 
    if (digitalRead(MODE_PIN) == HIGH)
    {
        if (da > 5)
        {
            cycle(-50);
            analog_value_prev = analog_value_current;
        }
        else if (da < -5)
        {
            cycle(50);
            analog_value_prev = analog_value_current;
        }
        else
        {
            cycle(0);
        }
    }
    else
    {
        Serial.print("delay_ms = ");
        Serial.println(delay_ms);
        cycle(delay_ms);
    }
}

