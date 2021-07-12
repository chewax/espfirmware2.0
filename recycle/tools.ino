/** Is this an IP? */
boolean isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    uint32_t c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (uint32_t i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}



char* toStringMac(uint8 mac [6])
{
    static char str [18];

    if(mac == NULL) return "";

    snprintf(str, sizeof(str), "%02x:%02x:%02x:%02x:%02x:%02x", 
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return str;
}

char* toStringMac(String mac)
{
    char *cstr = new char[mac.length() + 1];
    strcpy(cstr, mac.c_str());
    return cstr;
}

void logger (String msg, String delim) {
  USE_SERIAL.print("[");
  USE_SERIAL.print(delim);
  USE_SERIAL.print("] ");
  printTimestamp();
  USE_SERIAL.print(" ");
  USE_SERIAL.println(msg);
}

void printTimestamp () {
  time_t t = now();
  USE_SERIAL.print(day(t));
  USE_SERIAL.print("/");
  USE_SERIAL.print(month(t));
  USE_SERIAL.print("/");
  USE_SERIAL.print(year(t));
  USE_SERIAL.print(":");
  USE_SERIAL.print(hour(t));
  printDigits(minute(t));
  printDigits(second(t));
}

void printDigits(uint32_t digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  USE_SERIAL.print(":");
  if (digits < 10) Serial.print('0');
  USE_SERIAL.print(digits);
}

String asProperty(String key, String value, bool comma) {
    String result;

    result += "\"";
    result += key;
    result += "\":\"";
    result += value;
    result += "\"";
    if (comma)
        result += ",";

    return result;
}

String asJSONObj(String key) {
    String result;
    
    result += "{";
    result += key;
    result += "}";

    return result;
}