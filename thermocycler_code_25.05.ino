/* Hello!
 * This file contains the code to run the DIY thermocyler you've just made
 * 
 * @author: iGEM Leuven <info@igemleuven.be>
 * 
 * To start, look at your Arduino board and define the connections. 
 * Look at the number on the board next to the wires you've connected.
 * Notice that for analog inputs you must include the 'A' precedeing the pin number!
 */
int LED = 5;
int peltier = 6;
double temperatureSensor = A3;

/* In electronics we always have some unwated noise.
 * It can be visible on any measurment diagram where the signal has many small spikes.
 * To smooth them out, we average several adjecent readings.
 * Here, 6 consecutive readings would be stored in an array and later on averaged into 'avg_temperature'.
 */
int readings[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float avg_temperature;

void setup() {
  // Sets the data rate to 9600 bits per second, to run once:
  Serial.begin(9600);         

  // Set Arduino pins as output/input
  pinMode(LED, OUTPUT);
  pinMode(peltier, OUTPUT);
  pinMode(temperatureSensor, INPUT);
}

/* Some useful Arduino functions:
 * digitalWrite - writes HIGH or LOW value to a pin
 * digitalRead - reads HIGH or LOW value from a pin
 * analogWrite - writes an analog value (=continous value so eg. 0.23529 instead of just 0 and 1) to a pin
 * analogRead - reads the value from a pin 
 * It converts received input voltages between 0-5V into integer values between 0 and 1023.
 * That's why we used 'int' (=integer) to define our variables
 * 
 * Now, put your main code here, to be run repeatedly (=to loop):
 */
void loop() {
  digitalWrite(LED, HIGH); //HIGH means: provide voltage
  takeReading();
  delay(10);
  //switch on the Peltier element when temperature is lower than set value (50°C)
  if(avg_temperature<30){ 
    digitalWrite(peltier, HIGH);
    Serial.print(digitalRead(peltier));
    Serial.print("The temperature is: ");
    Serial.print(avg_temperature);
    Serial.println("°C");
    }
    else{
      digitalWrite(peltier, LOW);
      }
      
  digitalWrite(LED, LOW);
  Serial.print(" Test ");
}

// Function to calculate the resistance of the thermistor(=temperature sensor)
float calculateR(float value) //value is read from the temperature sensor
{
  float resistance = (10000*(value))/(1023-value);
  //10 000 is resistance in Ohms, 1023 is the maximum value that analogRead() can give 
  //Serial.print("The resistance is: "); 
  //Serial.println(resistance);
  return resistance; //Rx of thermistor
  }

// Function to calculate temperature from resistance based on equation for b at two temperatures (measured and 25°C)
float calculateT(float resistor)
{
  float temp = log(10000/resistor); //in c++ log actually means ln
  //10 000 is Ro (nominal sesnor resistance at 25°C) from datasheet
  temp = temp/3977;              
  /* 3977 is B (beta) value from datasheet
   * B value descibes the relationship between temperature and resistance
   * Resistance decreases in a non-linear way with temperature and B value accounts for the shape of this curve
   */
  temp = temp + 1/(25+273.15);
  temp = 1/temp;
  temp = temp-273.15; //convert back to degrees Celcius from Kelvin
  return temp;
  }

//function to calculate the temperature and store it back in the array 'readings'
  void takeReading()
{
  for(int i = 0; i<8; i++)
  {
    readings[i] = analogRead(temperatureSensor);  //sample the voltages
    delay(500); //wait 200ms
    }

  //convert to temperature for each sample
    for(int i = 0; i<8; i++)
  {
    readings[i] = calculateT(calculateR(readings[i]));
    }
    
  float avg = 0;

//sum the readings up to a total
 for(int i = 0; i<8; i++)
  {
    avg = avg + readings[i];
    }

    avg = avg/8; //take average of the 6 readings
    avg_temperature = avg-6; //6 is just an optimisation factor to account for non-ideal behaviour :) 
}
