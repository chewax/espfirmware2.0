//Initializes Serial
void initializeSerial()
{
    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(true);
    USE_SERIAL.println('[SETUP] SERIAL INITIALIZED');
}