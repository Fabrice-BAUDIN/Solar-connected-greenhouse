void garden_gest(){
  if(bitRead(refresh_demand,REFRESH_DATA))
  { 
    onewire_garden.select(address_soil_garden);
    temperature_soil_garden=onewire_garden.getTempC();
    read_values_out();
    read_values_compost();
    tension_battery();
    intensity_battery();
    intensity_load();
    get_moisture_garden();
    cat_proof_control();

    

    //spray_control_compost(); Automatic mode is not use currently
    temperature_control_compost();
    spray_control_garden();    
  }
  
  
}

void read_values_compost(){
  temperature_compost=compost_sensor.readTempC();
  pressure_compost=compost_sensor.readFloatPressure();
  humidity_compost=compost_sensor.readFloatHumidity();
}

void read_values_out(){
  temperature_out=out_sensor.readTempC();
  pressure_out=(out_sensor.readFloatPressure())/100;
  humidity_out=out_sensor.readFloatHumidity();

  // take samples in one hour to do average , use for the weather forecasts
  
  pressure_corrected= pressure_out * pow((1 - ((0.0065 * set_altitude) / (temperature_out + 0.0065 * set_altitude + 273.15))), -5.257);
  if (current_sample_pressure<total_samples_pressure){current_sample_pressure++;current_average_pressure=current_average_pressure+(pressure_corrected/total_samples_pressure);}
  else{
    rotate_index++;
    if (rotate_index>NUMBER_SAMPLES_HOUR){rotate_index=0;}
    average_pressure[rotate_index]= current_average_pressure;
    current_average_pressure=0;
    current_sample_pressure=0;    
  }
}



float tension_battery(){
  float read_pin=0;
  for (int i = 0; i <100; i ++) {
  read_pin = read_pin +float( analogRead(V_BATTERY))/100; 
  }
  //float read_pin= float(analogRead(V_BATTERY));
  v_battery= 1+((read_pin*30/1023)/1.07)+ float(set_v_offset_battery)/100;
  return v_battery;
  
}

float intensity_battery(){
  float read_pin=0;
  for (int i = 0; i <500; i ++) {
  read_pin = read_pin +float( analogRead(INTENSITY_BATTERY))/500; 
  }
  read_pin=read_pin-512;
  //float read_pin= float(analogRead(INTENSITY_BATTERY)-512);
  a_battery= (read_pin*25/512)+ float(set_a_offset_battery)/100;   //100 mV/A
  return a_battery;

}

float intensity_load(){
  float read_pin=0;
  for (int i = 0; i <500; i ++) {
  read_pin = read_pin +float( analogRead(INTENSITY_LOAD))/500; 
  }
  read_pin=read_pin-512;
  //float read_pin=float(analogRead(INTENSITY_LOAD)-512);  
  a_load= (read_pin*25/512)+ float(set_a_offset_load)/100;
  return a_load;

}

  
void spray_control_compost(){
  if (millis()-last_cleaning_compost>delay_cleaning_compost)
      {
        if (!start_cleaning_compost==1) // first step cleaning tanck
          { start_cleaning_compost=1; delay_start_spray_compost=millis(); bitSet(output_garden,SPRAY_COMPOST);}
        else if (millis()- delay_start_spray_compost > delay_spray_compost)
          {bitClear(output_garden,SPRAY_COMPOST);}
  
        if(!bitRead(output_garden,SPRAY_COMPOST)&& !start_pump_compost==1) // second step pump water
          { start_pump_compost=1; delay_start_pump_compost=millis();bitSet(output_garden,PUMP_COMPOST);}
        else if(!bitRead(output_garden,PUMP_COMPOST) &&  (millis() - delay_start_pump_compost > delay_pump_compost))
          {bitClear(output_garden,PUMP_COMPOST); last_cleaning_compost=millis(); }        
      }
}

void temperature_control_compost(){
    if ((temperature_compost<set_temperature_compost))
      {
        bitSet(output_garden,HEATING_COMPOST);
      }
    else{
        bitClear(output_garden,HEATING_COMPOST);
    }
  }

void get_moisture_garden(){
  //int read_pin=analogRead(MOISTURE_GARDEN);
  float read_pin=0;
  for (int i = 0; i <500; i ++) {
  read_pin = read_pin +float( MOISTURE_GARDEN)/500; 
  }
  //read_pin=read_pin-80;
  //moisture_garden= 100-(read_pin*10/90);
  moisture_garden=int(float(set_k_moist_garden*100)/read_pin);
  if(moisture_garden>100){moisture_garden=100;}
  else if(moisture_garden<0){moisture_garden=0;}
  return moisture_garden;}

void spray_control_garden(){

  if(moisture_garden<set_moisture_garden and night_day==1){
    if(start_timer_spray_garden==0)
      {start_timer_spray_garden=1; start_timer_spray_garden=millis();bitSet(output_garden,SPRAY_GARDEN); }
    else if (millis()-start_timer_spray_garden >DELAY_SPRAY and start_timer_spray_garden==1)
      {start_timer_spray_garden=2;start_timer_spray_garden=millis();bitClear(output_garden,SPRAY_GARDEN);}
    else if (millis()-start_timer_spray_garden >DELAY_PAUSE_SPRAY and start_timer_spray_garden==2)
      {start_timer_spray_garden=0;}    
   } 
 else{bitClear(output_garden,SPRAY_GARDEN);start_timer_spray_garden=0;}
}

void cat_proof_control(){
  if(digitalRead(CAT_PROOF_DETECT)){
    bitSet(output_garden,CAT_PROOF_GARDEN);
  }
  else{bitClear(output_garden,CAT_PROOF_GARDEN);}
}


  
