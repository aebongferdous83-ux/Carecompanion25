void huskyy() {
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS!"));
  else if (!huskylens.isLearned()) Serial.println(F("Nothing learned yet!"));
  else if (!huskylens.available()) Serial.println(F("No tag detected!"));
  else {
    Serial.println(F("-------- Detected Tags --------"));
    while (huskylens.available()) {
      HUSKYLENSResult result = huskylens.read();
      printResult(result);
    }
  }
}


void printResult(HUSKYLENSResult result) {
  if (result.command == COMMAND_RETURN_BLOCK) {
    Serial.print(F("Tag ID: "));
    Serial.println(result.ID);  // Unique ID of the tag
    Serial.print(F("X: "));
    Serial.print(result.xCenter);
    Serial.print(F(", Y: "));
    Serial.println(result.yCenter);
    Serial.print(F("Width: "));
    Serial.print(result.width);
    Serial.print(F(", Height: "));
    Serial.println(result.height);


    if (result.ID == med_cat) {
      myDFPlayer.play(3);
      alarm_done = 1;
      
    }

    else {
      myDFPlayer.play(2);
      delay(3000);
    }
  }
}