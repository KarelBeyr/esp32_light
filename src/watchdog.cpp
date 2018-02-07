#include "watchdog.h"
#include "esp_system.h"
#include <Arduino.h>

hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule()
{
  esp_restart_noos();
}

void setupWatchdog()
{
  timer = timerBegin(0, 80, true);  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);
  timerAlarmWrite(timer, 3000000, false);   //set time in us
  timerAlarmEnable(timer);  //enable interrupt
}

void feedWatchdog()
{
  timerWrite(timer, 0);
}
