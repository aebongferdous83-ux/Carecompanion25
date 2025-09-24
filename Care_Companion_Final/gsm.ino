void sendSMS(String message) {
    Serial.println("AT+CMGS=\"+8801759422284\"");
    delay(100);
    Serial.print(message);
    delay(100);
    Serial.write(26);
    Serial.println("Message Sent!");
}

void call_phone() {
  Serial.println("ATD+8801685696928;");
}

void sms_prep() {
  if (step) {
    if (millis() - prep_time > 10000 && step == 1) {
      prep_time = millis();
      Serial.println("AT");
      step = 2;
    } 
    
    else if (millis() - prep_time > 1000 && step == 2) {
      Serial.println("AT+CMGF=1");
      step = 0;
    }
  }
}