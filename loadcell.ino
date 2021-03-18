include <HX711.h>

HX711 cell(3, 4);
void setup() {
  Serial.begin(9600);
}
long val=0;
float count=0;
void loop(){
  //count=count+1;
  //val=((count-1)/count)*val + (1/count)* cell.read();
  val = cell.read();
  Serial.println(val );
  //55440
}
