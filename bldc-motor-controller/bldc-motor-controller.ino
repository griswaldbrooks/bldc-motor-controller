/* BLDC Motor Controller
 */

// The choice of pins are influenced by which arduino is used, in this case
// the uno, for consistent frequency (I hope)
const unsigned int PHASE_A(9);
const unsigned int PHASE_B(10);
const unsigned int PHASE_C(11);
const unsigned int MODE_PIN(7);

void setup() 
{
    pinMode(PHASE_A, OUTPUT);
    pinMode(PHASE_B, OUTPUT);
    pinMode(PHASE_C, OUTPUT);
    pinMode(MODE_PIN, INPUT);
    TCCR1B = (TCCR1B & 0b11111000) | 0x01;
    TCCR2B = (TCCR2B & 0b11111000) | 0x01;
    Serial.begin(115200);
}

const unsigned int PHASE_SET[6][3] = {
    {200, 200,   0},
    {200,   0,   0},
    {200,   0, 200},
    {  0,   0, 200},
    {  0, 200, 200},
    {  0, 200,   0}
};

void trap(float t, bool enable)
{
    if (enable)
    {
        float pa(0.5 * (sin(t) + 1.f));
        float pb(0.5 * (sin(t + 2 * PI / 3) + 1.f));
        float pc(0.5 * (sin(t + 4 * PI / 3) + 1.f));
        analogWrite(PHASE_A, static_cast<unsigned int>(255.f * pa));
        analogWrite(PHASE_B, static_cast<unsigned int>(255.f * pb));
        analogWrite(PHASE_C, static_cast<unsigned int>(255.f * pc));
    }
    else
    {
        analogWrite(PHASE_A, 0);
        analogWrite(PHASE_B, 0);
        analogWrite(PHASE_C, 0);
    }
}

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
        analogWrite(PHASE_A, 0);
        analogWrite(PHASE_B, 0);
        analogWrite(PHASE_C, 0);
        return;
    }
    
    Serial.print("PHASE_SET ndx = ");
    Serial.println(ndx);
    
    analogWrite(PHASE_A, PHASE_SET[ndx][0]);
    analogWrite(PHASE_B, PHASE_SET[ndx][1]);
    analogWrite(PHASE_C, PHASE_SET[ndx][2]);
    
    delay(abs(delay_ms));
}

void loop()
{
    static int analog_value_prev(0);
    int analog_value_current = analogRead(0);
    int da = analog_value_current - analog_value_prev;
    //Serial.print("analog_value_prev = ");
    //Serial.println(analog_value_prev);
    //Serial.print("analog_value_current = ");
    //Serial.println(analog_value_current);
    //Serial.print("da = ");
    //Serial.println(da);

    int delay_ms(50.f * sin(millis() / 1000.f));
   
    // If mode is HIGH, do position control, else speed demo. 
    if (digitalRead(MODE_PIN) == HIGH)
    {
        
        if (da > 1)
        {
            //cycle(50);
            trap(7.f * 2.f * PI * analog_value_current / 1023.f, true);
            analog_value_prev = analog_value_current;
            delay(5);
        }
        else if (da < -1)
        {
            //cycle(-50);
            trap(7.f * 2.f * PI * analog_value_current / 1023.f, true);
            analog_value_prev = analog_value_current;
            delay(5);
        }
        else
        {
            //cycle(0);
            trap(0, false);
        }
        
    }
    else
    {
        //Serial.print("delay_ms = ");
        //Serial.println(delay_ms);
        //cycle(delay_ms);
        static float t(0);
        trap(t, true);
        t += 0.001;
    }
}

