void mpu_read() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  if ((millis() - last_fall) > 200) {
    // Read raw accelerometer and gyroscope data
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Convert to 'g' force
    float axg = ax / 16384.0;
    float ayg = ay / 16384.0;
    float azg = az / 16384.0;

    // Calculate total acceleration magnitude
    float totalAccel = sqrt(axg * axg + ayg * ayg + azg * azg);

    // Calculate tilt angle (using Z-axis reference)
    float angle = atan2(sqrt(axg * axg + ayg * ayg), azg) * 180.0 / PI;

    // Check fall conditions
    if (totalAccel > 2.0 && angle < 80 && fallDetected == 0) {
      fallDetected = true;
      fall_pause = millis();
      int randValue = random(5, 9);  // upper bound is exclusive
      myDFPlayer.play(randValue);
      Serial.println("⚠️ Fall detected!");
      alert.updateAndReportParam("Fall Detect", (bool)1);
    }

    else if (fallDetected && millis() - fall_pause > 7000) {
      fallDetected = false;
      digitalWrite(buzzer, 0);
      alert.updateAndReportParam("Fall Detect", (bool)0);
    }

    else if (fallDetected && millis() - fall_pause > 5000) digitalWrite(buzzer, 1);
    last_fall = millis();
  }
}