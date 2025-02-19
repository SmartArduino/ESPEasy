//#######################################################################################################
//#################################### Plugin 022: PCA9685 ##############################################
//#######################################################################################################

#define PLUGIN_022
#define PLUGIN_ID_022         22
#define PLUGIN_NAME_022       "PWM - PCA9685"
#define PLUGIN_VALUENAME1_022 "PWM"

#define PCA9685_MODE1   0x00  // location for Mode1 register address 
#define PCA9685_MODE2   0x01  // location for Mode2 reigster address 
#define PCA9685_LED0    0x06  // location for start of LED0 registers 
#define PCA9685_ADDRESS 0x40  // I2C address

boolean Plugin_022_init = false;

boolean Plugin_022(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;
  static byte switchstate[TASKS_MAX];

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_022;
        Device[deviceCount].Type = DEVICE_TYPE_I2C;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].ValueCount = 0;
        Device[deviceCount].Custom = true;
        Device[deviceCount].TimerOption = false;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_022);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_022));
        break;
      }

    case PLUGIN_WRITE:
      {
        if (!Plugin_022_init)
        {
          // default mode is open drain ouput, drive leds connected to VCC
          Plugin_022_writeRegister(PCA9685_MODE1, (byte)0x01); // reset the device
          delay(1);
          Plugin_022_writeRegister(PCA9685_MODE1, (byte)B10100000);  // set up for auto increment
          Plugin_022_writeRegister(PCA9685_MODE2, (byte)0x10); // set to output
          Plugin_022_init = true;
        }
        String tmpString  = string;
        int argIndex = tmpString.indexOf(',');
        if (argIndex)
          tmpString = tmpString.substring(0, argIndex);
        if (tmpString.equalsIgnoreCase("PCAPWM"))
        {
          success = true;
          Plugin_022_Write(event->Par1, event->Par2);
          if (printToWeb)
          {
            printWebString += F("PCAPWM ");
            printWebString += event->Par1;
            printWebString += F(" Set to ");
            printWebString += event->Par2;
            printWebString += F("<BR>");
          }
        }
        break;
      }
  }
  return success;
}


//********************************************************************************
// PCA9685 config
//********************************************************************************
void Plugin_022_writeRegister(int regAddress, byte data) {
  Wire.beginTransmission(PCA9685_ADDRESS);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}


//********************************************************************************
// PCA9685 write
//********************************************************************************
boolean Plugin_022_Write(byte Par1, int Par2)
{
  boolean success = false;
  uint16_t LED_ON = 0;
  uint16_t LED_OFF = Par2;
  Wire.beginTransmission(PCA9685_ADDRESS);
  Wire.write(0x06 + 4 * Par1);
  Wire.write(lowByte(LED_ON));
  Wire.write(highByte(LED_ON));
  Wire.write(lowByte(LED_OFF));
  Wire.write(highByte(LED_OFF));
  Wire.endTransmission();
}

