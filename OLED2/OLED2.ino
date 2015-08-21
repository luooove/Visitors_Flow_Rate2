#include "OLED.h"
#include "stdlib.h"

 void setup()
 {
   LEDPIN_Init();
   LED_Init();
 }

 void loop()
 {
   int i;
   String s1="sfa";
  // s1+=i;
    LED_P8x16Str(40,2,s1);
    LED_P8x16Str(20,4,"Prototyping");
    delay(200);
    i++;
 }
