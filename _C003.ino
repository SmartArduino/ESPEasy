//#######################################################################################################
//########################### Controller Plugin 003: Nodo Telnet  #######################################
//#######################################################################################################

#define CPLUGIN_003
#define CPLUGIN_ID_003         3
#define CPLUGIN_NAME_003       "Nodo Telnet"

boolean CPlugin_003(byte function, struct EventStruct *event)
{
  boolean success = false;

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
      {
        Protocol[++protocolCount].Number = CPLUGIN_ID_003;
        strcpy_P(Protocol[protocolCount].Name, PSTR(CPLUGIN_NAME_003));
        Protocol[protocolCount].usesMQTT = false;
        Protocol[protocolCount].usesAccount = false;
        Protocol[protocolCount].usesPassword = true;
        Protocol[protocolCount].defaultPort = 23;
        break;
      }

    case CPLUGIN_PROTOCOL_SEND:
      {
        char log[80];
        boolean success = false;
        char host[20];
        sprintf_P(host, PSTR("%u.%u.%u.%u"), Settings.Controller_IP[0], Settings.Controller_IP[1], Settings.Controller_IP[2], Settings.Controller_IP[3]);

        sprintf_P(log, PSTR("%s%s using port %u"), "TELNT: connecting to ", host,Settings.ControllerPort);
        addLog(LOG_LEVEL_DEBUG, log);
        if (printToWeb)
        {
          printWebString += log;
          printWebString += "<BR>";
        }
        // Use WiFiClient class to create TCP connections
        WiFiClient client;
        if (!client.connect(host, Settings.ControllerPort))
        {
          connectionFailures++;
          strcpy_P(log, PSTR("TELNT: connection failed"));
          addLog(LOG_LEVEL_ERROR, log);
          if (printToWeb)
            printWebString += F("connection failed<BR>");
          return false;
        }
        statusLED(true);
        if (connectionFailures)
          connectionFailures--;

        float value = UserVar[event->BaseVarIndex];
        // We now create a URI for the request
        String url = F("variableset ");
        url += event->idx;
        url += ",";
        url += value;
        url += "\n";

        strcpy_P(log, PSTR("TELNT: Sending enter"));
        addLog(LOG_LEVEL_ERROR, log);
        client.print(" \n");

        unsigned long timer = millis() + 200;
        while (!client.available() && millis() < timer)
          delay(1);

        timer = millis() + 1000;
        while (client.available() && millis() < timer && !success)
        {
          String line = client.readStringUntil('\n');
          //Serial.println(line);
          if (line.substring(0, 20) == "Enter your password:")
          {
            success = true;
            strcpy_P(log, PSTR("TELNT: Password request ok"));
            addLog(LOG_LEVEL_ERROR, log);
          }
          delay(1);
        }

        strcpy_P(log, PSTR("TELNT: Sending pw"));
        addLog(LOG_LEVEL_ERROR, log);
        client.println(SecuritySettings.ControllerPassword);
        delay(100);
        while (client.available())
          client.read();

        strcpy_P(log, PSTR("TELNT: Sending cmd"));
        addLog(LOG_LEVEL_ERROR, log);
        client.print(url);
        delay(10);
        while (client.available())
          client.read();

        strcpy_P(log, PSTR("TELNT: closing connection"));
        addLog(LOG_LEVEL_DEBUG, log);
        if (printToWeb)
          printWebString += F("closing connection<BR>");

        client.stop();

        break;
      }

  }
  return success;
}

