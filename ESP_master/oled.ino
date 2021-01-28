void newDisplayElement(int x, int y, String data) {
  newDisplayElement(left, x, y, data);
}

void newDisplayElement(int alignment, int x, int y, String data) {
  element[element_counter].aligned = alignment;
  element[element_counter].x = x;
  element[element_counter].y = y;
  element[element_counter].data = data;
  element_counter++;
  loadDisplay();
}

void updateDisplayElement(int number, String new_data) {
  if (element[number].data != new_data) {
    element[number].data = new_data;
    loadDisplay();
  }
}

void loadDisplay() {
  display.clear();
  for (int i = 0; i < element_counter; i++) {
    if (element[i].aligned == left) {
      display.setTextAlignment(TEXT_ALIGN_LEFT);
    }
    else if (element[i].aligned == right) {
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
    }
    else if (element[i].aligned == center) {
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    }
    display.drawString(element[i].x, element[i].y, element[i].data);
  }
  display.display();
}


void interfaceSelector() {
  switch (interface) {
    case 0: {
        pin();
        break;
      }
    case 1: {
        timeTrack();
        break;
      }
      case 2:{
        logInterface();
        break;
      }
  }
}


String temporaneous_pin;
void pin() {
  String message;
  if (interface != loaded_interface) {
    loaded_interface = interface;
    element_counter = 0;
    temporaneous_pin = "_";
    d = 0;
    e = -1;
    message = "There are 5 more tryes\nbefore erasing everything";
    f = EEPROM.read(1);
    message[10] = char(48 + (5 - f));
    if (f == 0) {
      newDisplayElement(center, 64, 20, "Insert the pin");
    }
    else if (f == chances + 1){
      newDisplayElement(center, 64, 20, "Chose your own pin");
    }
    else {
      newDisplayElement(center, 64, 20, message);
    }
    newDisplayElement(center, 64, 45, temporaneous_pin);
  }

  if (debouncedButtons() == up) {
    e = (e + 1) % 10;
    temporaneous_pin[d] = char(48 + e);
  }
  else if (debouncedButtons() == down) {
    e = (e + 9) % 10;
    temporaneous_pin[d] = char(48 + e);
  }
  else if (debouncedButtons() == confirm) {
    if (temporaneous_pin[d] == '_' || d == 16) {
      String data;
      for (q = 0; temporaneous_pin[q] != '_'; q += 2) {
        data += temporaneous_pin[q];
      }
      data += '\0';
      Serial.print("Inserted key = ");
      Serial.println(data);
      setMasterKey(data);
      loadSector(1);
      if (wrong_key) {
        wrong_key = false;
        f++;
        if (f > 4) {
          eepromClear();
          ESP.restart();
        }
        else {
          EEPROM.write(1, f);
          EEPROM.commit();
          temporaneous_pin = "_";
          d = 0;
          message = "There are 5 more tryes\nbefore erasing everything"; //??????????
          message[10] = (char)(48 + (5 - f));
          updateDisplayElement(0, message);
          Serial.print("Message = ");
          Serial.println(message);
        }
      }
      else {
        EEPROM.write(1, 0);
        EEPROM.commit();
        interface = 1;
      }
    }
    else {
      temporaneous_pin += " _";
      d += 2;
    }
    e = -1;
    while (debouncedButtons() == confirm);
  }
  updateDisplayElement(1, temporaneous_pin);
}

void timeTrack() {
  if (interface != loaded_interface) {
    loaded_interface = interface;
    element_counter = 0;
    newDisplayElement(1, 1, String (millis() / 1000));
    newDisplayElement(right, 128, 52, wifi_IP);
  }
  if (ota_initialised){
    updateDisplayElement(1, "OTA " + wifi_IP);
  }
  updateDisplayElement(0, String (millis() / 1000));
}

void logInterface() {
  if (interface != loaded_interface) {
    loaded_interface = interface;
    element_counter = 0;
    newDisplayElement(left, 1, 1, "connected to");
    newDisplayElement(right, 128, 52, wifi_IP);

  }

}

void Dlog(String logtext){
  interface=2;
  interfaceSelector();
 updateDisplayElement(0, logtext);
}
