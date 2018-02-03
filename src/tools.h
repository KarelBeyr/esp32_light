#ifndef Tools_h
#define Tools_h
void blink(int times, int msHigh, int msLow);

class State
{
  public:
    State(int f, int d, int dl, int ml, bool a, float u, float df, int cd);
    int freq;
    int duty;
    int desiredLux;
    int measuredLux;
    bool automaticMode;
    float utlumStin;
    float dutyFor100lux;
    int currentDuty;

  private:
};
#endif