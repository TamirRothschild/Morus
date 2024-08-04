void setup()
{
  Serial.begin(9600);
  pinMode(A1, INPUT);
  pinMode(A0, INPUT);
  pinMode(A2, INPUT);
}


int moisture = 0;

int moisture_status = 0; // 0 is less; 1 is good; 2 is too much;

int temperature = 0;

int temperature_status = 0; // 0 is less; 1 is good; 2 is too much;


int light = 0;

int light_status = 0; // 0 is less; 1 is good; 2 is too much;


void loop(){
    delay(500);

// moisture section

    moisture = map(analogRead(A1), 1023, 0, 0, 100);

    if moisture >= 60 {
        moisture_status = 2; // too much mositure 
    }

    if moisture <= 7{
        moisture_status = 0; // too dry need more mositure
    }

    if (moisture > 7 && moisture < 60 ) {
        moisture_status = 1; // perfect mositure; too good for your shity plant; tho good weather good for you., 
    }


// temperature section

    temperature = map(analogRead(A0), 0, 1023, 0, 165);

    if (temperature > 26) {
        temperature_status = 2; // too hot to breath good luck staing alive;
    }

    if (temperature < 22) {
        temperature_status = 0; // it freezingggggg in here get some sun boyyyy;
    }

    if (temperature <= 26 && temperature >= 22) {
        temperature_status = 1; // this is what heaven feels like; too good to be true kinda weather., 
    }

// light section

 light = map(analogRead(A2), 0, 1023, 0, 100);

    if (light > 60) {
        Serial.println("too bright");  
    }
  
    if (light < 20) {
        Serial.println("too dim");
    }
  
    if (light <= 60 && light >= 20) {
        Serial.println("perfect light");
    }












}