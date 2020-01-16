/************************************************************************************
    
    <Automatic greenhouse and greengarden control,>
    Copyright (C) <2019>  <Fabrice BAUDIN>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    info@InnoGreenTech.fr

****************************************************************************************/

/*******************************
 *         utilities           *
 *******************************/
 
    #include <printf.h>
    #include <registers.h> 
    #include <pin_magic.h>

/************************
 *   Set memory adress  *
 ************************/

    #include <EEPROM.h>
    
    #define MEM_SET                                         0
    #define MEM_SETTING_TEMPEPARTURE_COMPOST                1       // Set of temperature of COMPOSTomposter
    #define MEM_SETTING_ECO_TEMPERATURE_COMPOST             3       // Set temperature of COMPOSTomposter in eco mode 
    #define MEM_SETTING_HUMIDITY_COMPOST                    5       // Set of humidity of COMPOSTomposter
    #define MEM_SETTING_MOISTURE_GARDEN                     7      // Set of moisture of garden  
    #define MEM_SETTING_MOISTURE_GREENHOUSE                 9      // Set of moisture of greenhouse
    #define MEM_SETTING_TEMPERATURE_GROUND_GREENHOUSE       11      // set of temperature of greenhouse green
    #define MEM_SETTING_HUMIDITY_GREENHOUSE                 13      // Set of humidity of greenhouse
    #define MEM_SETTING_CO2_GREENHOUSE                      15      // Set of CO2 of Greenhouse 
    #define MEM_SETTING_ECO_BATTERY                         17      // Set tension of batterie to enter in eco mode
    #define MEM_SETTING_SAFE_BATTERY                        19      // Set tension of batterie to enter in safe mode  
    #define MEM_SETTING_LUMINOSITY                          21      // Set luminosity detect night
    #define MEM_SETTING_DEEP_WATER                          23      // Set the deep of tank water
    #define MEM_SETTING_TEMPERATURE_GREENHOUSE              25      // Set the max temperature of the green house
    #define MEM_SETTING_MAX_INTENSITY                               27      // Set the max temperature of the green house

/****************************
 *      Screen setting      *
 ****************************/

    #include <Adafruit_GFX.h>                 // Disign library
    #include <Adafruit_TFTLCD.h>              //library of screen


//Setting the pins for the screen

    #define LCD_CS    A3                         // Chip Select goes to Analog 3
    #define LCD_CD    A2                         // Command/Data goes to Analog 2
    #define LCD_WR    A1                         // LCD Write goes to Analog 1
    #define LCD_RD    A0                         // LCD Read goes to Analog 0
    #define LCD_RESET A4                         // Not use


// Define the colors for the screen

    #define RGB565(R,G,B) ((((unsigned int)R&0xF8)<<8) | (((unsigned int)G&0xFC)<<3) | ((unsigned int)B>>3))  // Macro to convert couleur 24 bit
    
    #define BLACK   0x0000
    #define BLUE    0x001F
    #define RED     0xF800
    #define GREEN   0x07E0
    #define CYAN    0x07FF
    #define MAGENTA 0xF81F
    #define YELLOW  0xFFE0
    #define ORANGE  0xFD20 
    #define WHITE   0xFFFF
    #define BROWN   RGB565(165,175,143)
    
    #define FOND    0x0000                    // back soil color
    
    
    Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);


/************************************
 * create button for application    *
 ************************************/

    Adafruit_GFX_Button six_buttons[6]; 
                      
    char greenhouse_buttons[6][6]={"VMC","FanC","PumpC","Light","Heat","Spray"};
    
    #define VMC_GREENHOUSE                0           // Manual mode  of ventilation
    #define FAN_COOLING_GREENHOUSE        1           // Manual mode  of the cooling sytem
    #define PUMP_COOLING_GREENHOUSE       2           // Manual mode  of the cooling system
    #define LAMP_GREENHOUSE               3           // Manual mode  of the horticol light
    #define HEATING_GREENHOUSE            4           // Manual mode  of the soil of the green house
    #define SPRAY_GREENHOUSE              5           // Manual mode  of the humidity system

    
    char garden_buttons[6][7]={"GardS","CompoS","PumpM","HeatC","PumpC","HumGh"};
    
    #define SPRAY_GARDEN              0                    
    #define SPRAY_COMPOST             1          
    #define MAIN_PUMP                 2         
    #define HEATING_COMPOST           3
    #define PUMP_COMPOST              4
    #define HUMIDIFICATOR_GREENHOUSE  5
    #define CAT_PROOF_GARDEN          6
    
    Adafruit_GFX_Button four_buttons[4]; 
                      
    char main_buttons[4][9]={"Serre","Composte","Jardin","Infos"};
    #define GREENHOUSE        0           // Button for screen of greenhouse
    #define COMPOST           1           // Button for screen of compost
    #define GARDEN            2           // Button for screen of garden
    #define INFORMATION       3           // Button for screen of garden
    
    char setting_buttons[4][4]={" + ","OK"," - ","ESC"};
    #define LESS  2             // Button Moins
    #define OK    1             // Button OK
    #define MORE  0             // Button PLUS
    #define ESC   3             // Button END
    
    
    Adafruit_GFX_Button two_buttons[2];
    
    char second_buttons[2][8]={"Escape","Setting"}; 
    #define ESCAPE      0           // Button for screen of greenhouse
    #define SETTING     1           // Button for screen of compost
    
    char choice_buttons[2][5]={"UP","DOWN"}; 
    #define UP          0           // Button for screen of greenhouse
    #define DOWN        1           // Button for screen of compost
    
    
    
    int code_set;             // code of the value to set


/***********************
 *   Setting tactil    *
 * *********************/
 
    #include <TouchScreen.h>    // Library for touch screen
    
    #define YP A3               // must be an analog pin, use "An" notation!
    #define XM A2               // must be an analog pin, use "An" notation!
    #define YM 9                // can be a digital pin
    #define XP 8                // can be a digital pin
    
    #define TS_MINX 70          // min value of the position on x
    #define TS_MAXX 920         // max value of the position on x
    #define TS_MINY 70          // min value of the position on Y
    #define TS_MAXY 920         // min value of the position on z
    
    TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
    
    unsigned long button_delay;          // delay of push buttom
    #define BUTTON_DELAY 200    // delay filter for the buttom 
    
    #define REFRESH_SCREEN 5000
    #define REFRESH_SCREEN_SECONDS 5
    unsigned long refresh_datas=0;   // delay to refresh screen and data, every 10 seconds*
    byte          refresh_demand=1;
    
    int x=0;                    // value X touch
    int y=0;                    // value y touch
    int z=0;                    // intensity touch

 
/*****************************
 * Ram memory control screen *
 *****************************/



    byte flag_screen=B00000001;                   //flag for chose the screen
    byte flag_first_screen=B00000000;             //flag use to know if it is the first lecture of screen
    
    #define MAIN_SCREEN         0
    #define GREENHOUSE_SCREEN   1
    #define COMPOST_SCREEN      2
    #define GARDEN_SCREEN       3
    #define SETTING_SCREEN      4
    #define INFORMATION_SCREEN  7
    
    byte index_setting;                         //Index to change value or parameter
    byte new_index;
    byte new_setting;
    
    
    
    byte flag=0;                                 // flag for the system work
    #define MODE_CONSIGNE       0                // Use to set the value
    #define MODE_CONSIGNE_BK    1                // Use to set the value
    #define PUSH_BUTTON         2
    #define WINTER_MODE         3
    #define SPRING_MODE         4
    #define SUMMER_MODE         5
    #define AUTUMN_MODE         6
    #define ECO_MODE            7
    
    int index_sent=0;                           //allow to sent data one by one with PERIOD_SENT bettween each
    #define PERIOD_SENT 100                     //one information every 100 ms;
    unsigned long period_sent=0;                //control the last message
    unsigned long number_of_samples;            //Number of samples between two sender 

      


/****************************************************** 
 *      Keys of communication with ESP8266            *
 ******************************************************/

    #define CODE_COMMAND_INFO                           "$I$"
    #define CODE_COMMAND_READ                           "$R$" 
    #define CODE_COMMAND_WRITE                          "$W$"
     
    
    #define CODE_TEMPERATURE_COMPOST                    10     
    #define CODE_HUMIDITY_COMPOST                       11
    #define CODE_PUMP_COMPOST                           12
    #define CODE_HEAT_COMPOST                           13
    #define CODE_SETTING_TEMPERATURE_COMPOST            14
    #define CODE_SETTING_HUMIDITY_COMPOST               15 
    #define CODE_ECO_TEMPERATURE_COMPOST                16
    #define CODE_SPRAY_COMPOST                          17
    #define CODE_CLEAN_COMPOST                          18
    #define CODE_OUTPUT_GARDEN                          19
    
    
    #define CODE_TEMPERATURE_OUT                        20
    #define CODE_HUMIDITY_OUT                           21
    #define CODE_LEVEL_WATER_GARDEN                     22
    #define CODE_SPRAY_GARDEN                           23
    #define CODE_TEMPERATURE_SOIL_GARDEN                24
    #define CODE_MOISTURE_GARDEN                        25
    #define CODE_SETTING_MOISTURE_GARDEN                26
    
    
    #define CODE_TEMPERATURE_GREENHOUSE                 30
    #define CODE_HUMIDITY_GREENHOUSE                    31
    #define CODE_CO2_GREENHOUSE                         32
    #define CODE_LEVEL_WATER_GREENHOUSE                 33
    #define CODE_SPRAY_GREENHOUSE                       34
    #define CODE_LAMP_GREENHOUSE                        35
    #define CODE_HEAT_GREENHOUSE                        36
    #define CODE_FAN_COOLING_GREENHOUSE                 37
    #define CODE_PUMP_COOLING_GREENHOUSE                38
    #define CODE_TEMPERATURE_WATER_GREENHOUSE           39
    #define CODE_TEMPERATURE_SOIL_GREENHOUSE            40
    #define CODE_MOISTURE_GREENHOUSE                    41
    #define CODE_VMC_GREENHOUSE                         42
    #define CODE_FLAP_VENTILATION_GREENHOUSE            43
    #define CODE_HUMIDIFICATOR_GREENHOUSE               44
    #define CODE_SETTING_MOISTURE_GREENHOUSE            45
    #define CODE_SETTING_TEMPERATURE_SOIL_GREENHOUSE    46
    #define CODE_SETTING_HUMIDITY_GREENHOUSE            47
    #define CODE_SETTING_CO2_GREENHOUSE                 48
    #define CODE_OUTPUT_GREENHOUSE                      49
    
    #define CODE_DATE                                   50
    #define CODE_HEURE                                  51
    #define CODE_IP                                     52
    #define CODE_GATE_WAY                               53
    #define CODE_MAC                                    54
    #define CODE_PERIOD_SEND                            55
    
    #define CODE_AVERAGE_V_BATTERY                      60
    #define CODE_GENERAL_PUMP                           61
    #define CODE_AVERAGE_A_BATTERY                      62
    #define CODE_AVERAGE_A_LOAD                         63
    #define CODE_SETTING_ECO_BATTERIE                   64
    #define CODE_SETTING_SAFE_BATTERIE                  65

/**************************************************
 *                RAM Memory map                  *
 **************************************************/

/*input Modul Compost/garden*/

    float   temperature_compost;
    int     humidity_compost;
    float   pression_compost=1000;
    
    float   a_battery;
    float   a_load;
    float   v_battery;

    
    float   temperature_out;
    int     humidity_out;
    int     moisture_garden;
    float   temperature_soil_garden;
    int     level_water_garden;
    byte    cat_proof;

/*input modul greenhouse*/

    float   temperature_greenhouse;
    int     humidity_greenhouse;
    int     level_water_greenhouse;
    float   temperature_water_greenhouse;
    float   temperature_soil_greenhouse;
    int     moisture_greenhouse;
    int     co2_greenhouse;
    int     luminosity_greenhouse;
     


/* output modul greenhouse*/

    byte    output_greenhouse=0;          
    byte    forced_greenhouse=0;                  // control if the forced mode is activate
    byte    desactive_greenhouse=0;

    unsigned int time_heat_greenhouse;
    unsigned int time_cooling_greenhouse;
    unsigned int time_spray_greenhouse;
    unsigned int time_vmc_greenhouse;
    unsigned int time_lamp_greenhouse;
    unsigned int time_humidificator_greenhouse;

    int          flap_ventilation_greenhouse;
    
    
/* output modul garden mode */

    byte    output_garden=0;                   // a bit by output tor
    byte    forced_garden=0;
    byte    desactive_garden=0;

    unsigned int time_main_pump;
    unsigned int time_spray_compost;
    unsigned int time_heat_compost;
    unsigned int time_pump_compost;
    unsigned int time_spray_garden;
    
/* String for commnunication with ESP8266 */

    String  date;             
    String  hour;         
    String  ip_address;      
    String  gate_way; 
    String  mac_address;    

/* memory for timing washing compost cycle*/
 
    unsigned long  delay_cleaning_compost=7*24*3600*1000;     // delay betwin two cleaning
    unsigned long  delay_spray_compost=30*1000;
    unsigned long  delay_pump_compost=30;

    unsigned long  last_cleaning_compost=0;
    unsigned long  delay_start_spray_compost=0;
    unsigned long  delay_start_pump_compost=0;

    byte           start_cleaning_compost=0;
    byte           start_pump_compost=0;  

/* memory for  lamp of greenhouse */

   byte            night_day;                // 1 when the night is detect
   byte            light_use;
   unsigned long   time_lamp_day;

   #define         TIME_LAMP_DAY  3600000   // Time of work of lamp by day

/**************************************************
 *                RAM Memory for setting          *
 **************************************************/


    int     set_temperature_compost;
    int     set_eco_temperature_compost;
    int     set_humidity_compost; 
    int     set_moisture_garden;
    int     set_moisture_greenhouse;
    int     set_temperature_soil_greenhouse;
    int     set_temperature_greenhouse;
    int     set_humidity_greenhouse;
    int     set_co2_greenhouse;
    int     set_eco_batterie;
    int     set_max_intensity;
    int     set_safe_batterie; 
    int     set_luminosity;
    int     set_deep_water;
    
    int     setting;


/**************************************************
 *    Connected map of composter modul            *
 **************************************************/

    #define RX1   17     //Serial communication Serial2
    #define TX1   16     
    
    #define SCL1  SCL      //I²C BUS
    #define SDA1  SDA
    
    #define A1_0  A6      //Charge battery control
    
    #define A1_1  A7      //Analog input, you can use it like a digital input
    #define A1_2  A8
    #define A1_3  A9
    #define A1_4  A10
    
    #define I1_0  32      //Digital input
    #define I1_1  34
    #define I1_2  36
    #define I1_3  38
    #define I1_4  40
    
    #define O1_0  44      //pwm output connected on a mofset
    #define O1_1  31        //Power output connected on a mofset
    #define O1_2  33
    #define O1_3  29
    #define O1_4  35
    #define O1_5  27
    
    #define O1_6  37    //Output 5 volts, can be use for Servo or Steper
    #define O1_7  25
    #define O1_8  39
    #define O1_9  23
    
    #define O1_10 41   //Output 5 volts



/**************************************************
 *    Connected map of greenhouse modul           *
 **************************************************/
 
    #define RX2   15     //Serial communication Serial3
    #define TX2   14     
    
    #define SCL2  21    //I²C BUS
    #define SDA2  20
    
    #define A2_0  A11    //Charge battery control
    
    #define A2_1  A12    //Analog input, you can use it like a digital input
    #define A2_2  A13
    #define A2_3  A14
    #define A2_4  A15
    
    #define I2_0  22      //Digital input
    #define I2_1  24
    #define I2_2  26
    #define I2_3  28
    #define I2_4  30
    
    #define O2_0  43    //Power output connected on a mofset
    #define O2_1  42
    #define O2_2  45    //Pwm output
    #define O2_3  46    //Pwm output
    #define O2_4  47
    #define O2_5  48
    
    #define O2_6  49    //Output 5 volts, can be use for Servo or Steper
    #define O2_7  50
    #define O2_8  51
    #define O2_9  52
    
    #define O2_10 53   //Output 5 volts


 /*********************************************************************************************** 
  *                                   sensors define                                            *
  ***********************************************************************************************/

#include <DHT.h>
#include <OneWire.h>
#include <DS18B20.h>
#include <NewPing.h>
#include "SparkFunBME280.h"  
#include "Wire.h"            
#include "SPI.h"

  /* Greenhouse sensors*/

    DHT greenhouse_dht(I2_0,DHT22);     // Sensor for humidity and temperature
    
    #define V_BATTERY             A2_4
    #define MOISTURE_GREENHOUSE   A2_0
    #define LUMINOSITY_GREENHOUSE A2_1
    
    DS18B20 onewire_greenhouse(I2_4);  // define the bus of one Wire of the green_house card
    uint8_t address_soil_greenhouse[] ={ 0x28, 0x2F, 0xBC, 0x79, 0x97, 0x08, 0x03, 0x1F};
    uint8_t address_water_greenhouse[] = { 0x28, 0xBF, 0x7B, 0x79, 0x97, 0x05, 0x03, 0x54};

    //Broches sonar for level of water

    #define   TRIGGER_SONAR   I2_3// Broche TRIGGER
    #define   ECHO_SONAR      I2_2// Broche ECHO
    NewPing   water_level(TRIGGER_SONAR,ECHO_SONAR);

/* Garden and lompricomposte sensors */

    #define MOISTURE_GARDEN     A1_4
    #define INTENSITY_BATTERY   A1_1
    #define INTENSITY_LOAD      A1_2   
    #define CAT_PROOF_DETECT    I1_1
    
    DHT     outdoor_dht(I1_0,DHT21);    //Outdoor sensor
    DS18B20 onewire_garden(I1_4);       //define the bus of the one Wire of the garden
    uint8_t address_soil_garden[] = {  0x28,  0x7D,  0x97,  0x79,  0x97,  0x7,  0x3,  0xCF};

    BME280  compost_sensor;             // BME280 sensor

 /*********************************************************************************************** 
  *                                   output define                                             *
  ***********************************************************************************************/

/* Greenhouse card output*/

    #define GREENHOUSE_VMC           O2_2         
    #define GREENHOUSE_PUMP_COOLING  O2_3
    #define GREENHOUSE_FAN_COOLING   O2_4
    #define GREENHOUSE_LAMP          O2_1
    #define GREENHOUSE_HEATING       O2_0  
    #define GREENHOUSE_HUMIDIFICATOR O2_5 

/* Garden card output*/

    #define GARDEN_MAIN_PUMP        O1_0
    #define EV_GARDEN_SPRAY         O1_1
    #define EV_GREENHOUSE_SPRAY     O1_5
    #define EV_COMPOST_SPRAY        O1_4
    #define COMPOST_HEATING         O1_3
    #define COMPOST_DRAIN_PUMP      O1_2
