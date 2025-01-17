void greenhouse_gest(){
  if(bitRead(refresh_demand,REFRESH_DATA))
  {
             temperature_greenhouse= greenhouse_dht.readTemperature(); 
             humidity_greenhouse=float(greenhouse_dht.readHumidity()); 
             onewire_greenhouse.select(address_soil_greenhouse);
             temperature_soil_greenhouse=onewire_greenhouse.getTempC();
             onewire_greenhouse.select(address_water_greenhouse);
             temperature_water_greenhouse=onewire_greenhouse.getTempC();
             get_water_level();
             get_moisture_greenhouse();
             co2_greenhouse=readCO2();
             get_luminosity();
             day_detect();
             vmc_controller();
             cooling_system();
             control_greenhouse_spray();
             lamp_run();
  }

  }


 void cooling_system(){
    byte cooling_day=0;
    byte cooling_night=0;
    
    if(((temperature_greenhouse>temperature_water_greenhouse+10) && night_day==0)||(temperature_greenhouse>set_temperature_greenhouse-5 and temperature_water_greenhouse<temperature_greenhouse)){cooling_day=1;} // control cooling during the day
    else{cooling_day=0;}


    if (bitRead(mode_flag,ECO_MODE)){
          if((temperature_greenhouse<(temperature_water_greenhouse-(temperature_water_greenhouse-temperature_out)*2/3)and (temperature_water_greenhouse-temperature_greenhouse)>5))
          {cooling_night=1;}
      }
    else if (!bitRead(mode_flag,SAFE_MODE)){
          if((temperature_greenhouse<(temperature_water_greenhouse-(temperature_water_greenhouse-temperature_out)/4)and (temperature_water_greenhouse-temperature_greenhouse)>5))
          {cooling_night=1;}
          }
    if (cooling_night==0){ 
          if((temperature_water_greenhouse>temperature_greenhouse) and temperature_greenhouse<3)
          {cooling_night=1;}
      }
      
    if(cooling_day==1 || cooling_night==1){    
            bitSet(output_greenhouse,FAN_COOLING_GREENHOUSE);
            bitSet(output_greenhouse,PUMP_COOLING_GREENHOUSE);
      }   
    else{
            bitClear(output_greenhouse,FAN_COOLING_GREENHOUSE);
            bitClear(output_greenhouse,PUMP_COOLING_GREENHOUSE);    
    }
  }

void get_water_level(){

  int distance=water_level.convert_cm(water_level.ping_median());
 // Serial.print(distance);Serial.println(" cm");
  if (distance<set_deep_water and distance!=0){level_water_greenhouse = set_deep_water-distance;}
 
  return;

  }



void get_moisture_greenhouse(){
  
  float read_pin=analogRead(MOISTURE_GREENHOUSE);
  moisture_greenhouse= int(((read_pin*a_moist_greenhouse)-b_moist_greenhouse)/100);
  if(moisture_greenhouse>100){moisture_greenhouse=100;}
  else if(moisture_greenhouse<0){moisture_greenhouse=0;}
  return moisture_greenhouse;}

void control_greenhouse_spray(){

   if (night_day==1 and !wifi_spray_greenhouse){
    //bitClear(output_greenhouse,SPRAY_GREENHOUSE);
    greenhouse_spray_done=0;                                 // reset memory use,wait day morning to switch on spray
    delay_greenhouse_spray=int((average_temperature_greenhouse-10)*15);    
  }
  if ((delay_greenhouse_spray>0 || moisture_greenhouse<set_moisture_greenhouse)and night_day==0 and !greenhouse_spray_done){
    if(moisture_greenhouse<set_moisture_greenhouse){bitSet(output_greenhouse,SPRAY_GREENHOUSE);}else{bitClear(output_greenhouse,SPRAY_GREENHOUSE);}
    delay_greenhouse_spray= delay_greenhouse_spray-DELAY_REFRESH_SCREEN_SECONDS;
  }
  else if (night_day==0 and !greenhouse_spray_done){
    greenhouse_spray_done=1;
  }
  else if ( humidity_greenhouse<(set_humidity_greenhouse-20) || temperature_greenhouse>(set_temperature_greenhouse+10))
    {
      bitSet(output_greenhouse,SPRAY_GREENHOUSE); 
    }
  else if ( wifi_spray_greenhouse and delay_greenhouse_spray>0){
    bitSet(output_greenhouse,SPRAY_GREENHOUSE);
    delay_greenhouse_spray= delay_greenhouse_spray-DELAY_REFRESH_SCREEN_SECONDS;
  }
  else if (  wifi_spray_greenhouse ){
    wifi_spray_greenhouse=0;
  }
  else{bitClear(output_greenhouse,SPRAY_GREENHOUSE);}
  
}

void vmc_controller(){
  
  int calculate_humidity;
  int difference_humidity= humidity_greenhouse-set_humidity_greenhouse;
  
  if(humidity_out<95 and !bitRead(output_greenhouse,VMC_GREENHOUSE)){calculate_humidity=(humidity_out*temperature_out/temperature_greenhouse)+5;}
  else if (humidity_out<95){calculate_humidity=(humidity_out*temperature_out/temperature_greenhouse);}  
  else{calculate_humidity=100;}

  if (temperature_greenhouse>set_temperature_greenhouse){             // security to protect hot temperature
  bitSet(output_greenhouse,VMC_GREENHOUSE);
  servo_vmc.write(100);
  }
  else if( calculate_humidity<(humidity_greenhouse-5) && difference_humidity>5 && night_day==0){               // Control humidity only the day
        int open_servo= map(difference_humidity,0,10,10,100);
        if( open_servo<100){servo_vmc.write(open_servo);}else{servo_vmc.write(100);}
        if (difference_humidity>15 and (average_temperature_greenhouse>22 or temperature_out >= temperature_greenhouse)){bitSet(output_greenhouse,VMC_GREENHOUSE);}
        else {bitClear(output_greenhouse,VMC_GREENHOUSE);}
       }
  else if (co2_greenhouse>set_co2_greenhouse){
        int open_servo= map(co2_greenhouse,set_co2_greenhouse,1700,10,100);
        if( open_servo<100){servo_vmc.write(open_servo);}else{servo_vmc.write(100);}
        if (co2_greenhouse>set_co2_greenhouse+300 and (average_temperature_greenhouse>22 or temperature_out >= temperature_greenhouse)){bitSet(output_greenhouse,VMC_GREENHOUSE);}
        else {bitClear(output_greenhouse,VMC_GREENHOUSE);}
        
        }
  else {bitClear(output_greenhouse,VMC_GREENHOUSE);servo_vmc.write(10);}  

 }

void lamp_run(){
  if (night_day==0){
    bitClear(output_greenhouse,LAMP_GREENHOUSE);light_use=0;                                 // reset memory use,wait night to switch on the lamp
  }
  else if (!bitRead(output_greenhouse,LAMP_GREENHOUSE) and !light_use)
        {bitSet(output_greenhouse,LAMP_GREENHOUSE); time_lamp_day=millis();}
  else if ((millis()- time_lamp_day) > TIME_LAMP_DAY){
        bitClear(output_greenhouse,LAMP_GREENHOUSE); light_use=1;
   }      
  }

  
float get_luminosity(){
  
  float read_pin=analogRead(LUMINOSITY_GREENHOUSE); 
  read_pin= (read_pin*10/1023); 
  read_pin= read_pin*read_pin*read_pin;
  read_pin=1.8*read_pin;
  luminosity_greenhouse= int(read_pin);
    
  return luminosity_greenhouse;
}

void day_detect(){

  if (luminosity_greenhouse<50 and night_day==0){night_day=1;}
  else if (luminosity_greenhouse>200 and night_day==1){night_day=0;}
}


/* Use the water tank to catch the solar energy available */

void energy_saver(){
  //Serial.println(average_temperature_greenhouse);
  if (average_temperature_greenhouse<24){
    //Serial.println(pwm_heat_water_tank);
    //Serial.println(v_battery);
    if (v_battery>27.1 and a_battery>-0.5 and a_battery<0.5 and !bitRead(output_greenhouse,HEATING_WATER_TANK))
      {
        bitSet(output_greenhouse,HEATING_WATER_TANK);
        //Serial.println("set");
      }
    else if (v_battery<26.5)
       {
        bitClear(output_greenhouse,HEATING_WATER_TANK);
        pwm_heat_water_tank=0;
       // Serial.println("reset");
      }
    if (bitRead(output_greenhouse,HEATING_WATER_TANK))
      { 
        //tension_battery();
        //Serial.println("adjust");
        intensity_battery();
        if (a_battery<-0.7 and pwm_heat_water_tank>0 )
        {pwm_heat_water_tank=pwm_heat_water_tank-1;}
        else if(a_battery>-0.5 and pwm_heat_water_tank<255)
        {pwm_heat_water_tank= pwm_heat_water_tank+1;}  
        heating_water_tank(0);      
      }
  }
  } 
