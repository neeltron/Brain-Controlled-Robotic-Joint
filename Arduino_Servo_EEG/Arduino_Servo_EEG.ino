#define LED 13
#define BAUDRATE 57600
#define DEBUGOUTPUT 0
#define powercontrol 10
byte generatedChecksum = 0;
byte checksum = 0;
int payloadLength = 0;
byte payloadData[64] = {0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;
long lastReceivedPacket = 0;
boolean bigPacket = false;
void setup()
{
  Serial.begin(BAUDRATE);
}
byte ReadOneByte()
{
  int ByteRead;
  while (!Serial.available());
  ByteRead = Serial.read();
#if DEBUGOUTPUT
  Serial.print((char)ByteRead);
#endif
  return ByteRead;
}
void loop()
{
  if (ReadOneByte() == 170)
  {
    if (ReadOneByte() == 170)
    {
      payloadLength = ReadOneByte();
      if (payloadLength > 169)
        return;
      generatedChecksum = 0;
      for (int i = 0; i < payloadLength; i++)
      {
        payloadData[i] = ReadOneByte();
        generatedChecksum += payloadData[i];
      }
      checksum = ReadOneByte();
      generatedChecksum = 255 - generatedChecksum;
      if (checksum == generatedChecksum)
      {
        poorQuality = 200;
        attention = 0;
        meditation = 0;
        for (int i = 0; i < payloadLength; i++)
        {
          switch (payloadData[i])
          {
            case 2:
              i++;
              poorQuality = payloadData[i];
              bigPacket = true;
              break;
            case 4:
              i++;
              attention = payloadData[i];
              break;
            case 5:
              i++;
              meditation = payloadData[i];
              break;
            case 0x80:
              i = i + 3;
              break;
            case 0x83:
              i = i + 25;
              break;
            default:
              break;
          }
        }
#if !DEBUGOUTPUT
        if (bigPacket)
        {
          if (poorQuality == 0)
            digitalWrite(LED, HIGH);
          else
            digitalWrite(LED, LOW);
          Serial.print(String(attention)+" "+String(meditation));
          lastReceivedPacket = millis();
          Serial.print("\n");
        }
#endif
        bigPacket = false;
      }
      else {
        Serial.print("Checksum error!");
      }
    }
  }
}
