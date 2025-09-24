void text(String t, byte x, byte y) {
  oled.setCursor(x, y);
  oled.print(t);
}