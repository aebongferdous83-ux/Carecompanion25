// ------------------- GPS Parser -------------------
void parseNMEA(String sentence) {
  if (sentence.startsWith("$GNRMC") || sentence.startsWith("$GPRMC")) {
    // Split by commas
    int idx = 0;
    int lastIdx = 0;
    int field = 0;
    String fields[20]; // Max 20 fields in RMC

    while ((idx = sentence.indexOf(',', lastIdx)) != -1) {
      fields[field++] = sentence.substring(lastIdx, idx);
      lastIdx = idx + 1;
    }
    fields[field] = sentence.substring(lastIdx);

    String status = fields[2]; // "A"=valid, "V"=invalid
    if (status == "A") {
      String rawLat = fields[3];
      String ns = fields[4];
      String rawLon = fields[5];
      String ew = fields[6];

      double lat = convertToDecimal(rawLat, ns);
      double lon = convertToDecimal(rawLon, ew);

      Serial.print("Latitude: ");
      Serial.println(lat, 6);
      Serial.print("Longitude: ");
      Serial.println(lon, 6);
    } 
    
    else {
      Serial.println("No valid GPS fix yet...");
    }
  }
}

// Convert NMEA lat/long to decimal degrees
double convertToDecimal(String raw, String dir) {
  if (raw == "") return 0.0;

  double val = raw.toDouble();
  int degrees = (int)(val / 100);
  double minutes = val - (degrees * 100);
  double dec = degrees + minutes / 60.0;

  if (dir == "S" || dir == "W") dec = -dec;
  return dec;
}