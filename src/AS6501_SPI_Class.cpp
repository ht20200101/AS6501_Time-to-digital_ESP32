#include <AS6501_SPI_Class.h>

  // constructor: supply CS pin + SPI instance
  AS6501::AS6501(uint8_t csPin, SPIClass& spi)
    : _cs(csPin), _spi(spi),
      _settings(50'000'000, MSBFIRST, SPI_MODE0),
      _present(false), _dualMode(false)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
  }

  // flags
  bool AS6501::isPresent() { return _present; }
  bool AS6501::isDualMode() { return _dualMode; }

  inline void AS6501::csLow()  { digitalWrite(_cs, LOW ); }
  inline void AS6501::csHigh() { digitalWrite(_cs, HIGH); }

  // send single opcode
  void AS6501::sendOpcode(uint8_t op) {
    csLow();
    _spi.beginTransaction(_settings);
      _spi.transfer(op);
    _spi.endTransaction();
    csHigh();
  }

  // must call once after SPI.begin(...)
  void AS6501::powerOnReset() {
    sendOpcode(OPC_POWER);
    delay(5);
  }

  // preserve config + start conversion
  void AS6501::initMeasurement() {
    sendOpcode(OPC_INIT);
    delay(1);
  }

  // write single config register
  void AS6501::writeRegister(uint8_t addr, uint8_t val) {
    csLow();
    _spi.beginTransaction(_settings);
      _spi.transfer(OPC_WR_CONF | (addr & 0x1F));
      _spi.transfer(val);
    _spi.endTransaction();
    csHigh();
  }

  // read single config register
  uint8_t AS6501::readRegister(uint8_t addr) {
    csLow();
    _spi.beginTransaction(_settings);
      _spi.transfer(OPC_RD_CONF | (addr & 0x1F));
      uint8_t v = _spi.transfer(0x00);
    _spi.endTransaction();
    csHigh();
    return v;
  }

  // bulk-write 17 cfg bytes
  void AS6501::writeConfig(const uint8_t cfg[NREG]) {
    csLow();
    _spi.beginTransaction(_settings);
    _spi.transfer(OPC_WR_CONF);
    for (uint8_t i = 0; i < NREG; ++i)
      _spi.transfer(cfg[i]); 
    _spi.endTransaction();
    csHigh();
  }

  // bulk-verify 17 cfg bytes -> sets _present
  bool AS6501::verifyConfig(const uint8_t cfg[NREG]) {
    _present = true;
    csLow();
    _spi.beginTransaction(_settings);
     // --- Send spiopc_read_config Command over SPI Once ---
    // This command (opcode 0x40 with starting address 0) triggers a readout.
    _spi.transfer(OPC_RD_CONF);
    for (uint8_t i = 0; i < NREG; ++i) {
      uint8_t rVal = _spi.transfer(0x0);
      if (rVal != cfg[i]) {
        Serial.printf("Reg %d mismatch: wrote 0x%02X, read 0x%02X\n", i, cfg[i], rVal);
        _present = false;
      }
    }
     _spi.endTransaction();
    csHigh();
    return _present;
  }

  // configure single-channel (STOPA only)
  bool AS6501::configureSingleChannel() {
    powerOnReset();
    writeConfig(cfgSingle);
    //if (!verifyConfig(cfgSingle)) return false;
    while (!verifyConfig(cfgSingle)) {
        Serial.println("Configuration mismatch; re-checking in 1 second...");
        delay(1000);
    }
    initMeasurement();
    _dualMode = false;
    return true;
  }

  // configure dual-channel (STOPA + STOPB)
  bool AS6501::configureDualChannel() {
    powerOnReset();
    writeConfig(cfgDual);
    //if (!verifyConfig(cfgDual)) return false;
    while (!verifyConfig(cfgDual)) {
        Serial.println("Configuration mismatch; re-checking in 1 second...");
        delay(1000);
    }
    initMeasurement();
    _dualMode = true;
    return true;
  }

  // read 24-bit REFID or TSTOP
  uint32_t AS6501::readResult(uint8_t baseAddr) {
    uint32_t v = 0;
    csLow();
    _spi.beginTransaction(_settings);
      _spi.transfer(OPC_RD_RES | (baseAddr /*& 0x1F*/));
      v  = uint32_t(_spi.transfer(0x00)) << 16;
      v |= uint32_t(_spi.transfer(0x00)) << 8;
      v |= uint32_t(_spi.transfer(0x00));
    _spi.endTransaction();
    csHigh();
    return v;
  }
