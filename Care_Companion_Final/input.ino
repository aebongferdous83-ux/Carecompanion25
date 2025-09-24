void input_processing() {
  if (incoming_message) {
    incoming_message = 0;
    Serial.print(device_name);
    Serial.print(" ");
    Serial.print(param_name);
    Serial.print(" ");
    Serial.print(incoming_value);
    Serial.print(" ");
    Serial.println(received_message);

    if (device_name == "Alert") {
      if (param_name == "Reminder") {
        alarm_done = 0;
        med_cat = 1;
        digitalWrite(buzzer, 1);
        delay(2000);
        digitalWrite(buzzer, 0);
        delay(500);
        digitalWrite(buzzer, 1);
        delay(500);
        digitalWrite(buzzer, 0);
        while (!alarm_done) huskyy();
        alert.updateAndReportParam("Reminder", (bool)0);
      }

      else if (param_name == "Morning") {
        alarm_done = 0;
        med_cat = 2;
        digitalWrite(buzzer, 1);
        delay(2000);
        digitalWrite(buzzer, 0);
        delay(500);
        digitalWrite(buzzer, 1);
        delay(500);
        digitalWrite(buzzer, 0);
        while (!alarm_done) huskyy();
        alert.updateAndReportParam("Morning", (bool)0);
      }

      else if (param_name == "Afternoon") {
        alarm_done = 0;
        med_cat = 3;
        digitalWrite(buzzer, 1);
        delay(2000);
        digitalWrite(buzzer, 0);
        delay(500);
        digitalWrite(buzzer, 1);
        delay(500);
        digitalWrite(buzzer, 0);
        while (!alarm_done) huskyy();
        alert.updateAndReportParam("Afternoon", (bool)0);
      }

      else if (param_name == "Night") {
        alarm_done = 0;
        med_cat = 4;
        digitalWrite(buzzer, 1);
        delay(2000);
        digitalWrite(buzzer, 0);
        delay(500);
        digitalWrite(buzzer, 1);
        delay(500);
        digitalWrite(buzzer, 0);
        while (!alarm_done) huskyy();
        alert.updateAndReportParam("Night", (bool)0);
      }
    }
  }
}