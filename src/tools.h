#ifndef Tools_h
#define Tools_h
void blink(int times, int msHigh, int msLow);

class State
{
  public:
//    State(int freq, int duty, int desiredLux, int measuredLux, bool automaticMode, float utlumStin, float dutyFor100lux);
  State(int f, int d, int dl, int ml, bool a, float u, float df);
    int freq;
    int duty;
    int desiredLux;
    int measuredLux;
    bool automaticMode;
    float utlumStin;
    float dutyFor100lux;
  private:
};
#endif