MIT License

Copyright (c) 2025 hhuzefa T.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


// AS6501.h
// Arduino-IDE driver for ScioSense AS6501 Time-to-Digital Converter
// – ESP32-S3 (FSPI) or any Arduino SPI
// – REFCLK_DIVISIONS = 10 000 → 10 MHz ref clock
// – Single vs Dual STOPA/B modes
// – CFG2 = 0xC0, CMOS inputs, CFG8…15 = datasheet defaults
// – User provides SPI instance (no SPI.begin inside class)
// – Verify config & presence flag, mode flag

#ifndef AS6501_H
#define AS6501_H

#include <Arduino.h>
#include <SPI.h>

class AS6501 {
public:
  // number of configuration registers
  static constexpr uint8_t NREG = 17;

  // SPI opcodes
  static constexpr uint8_t OPC_POWER   = 0x30;  // power-on reset
  static constexpr uint8_t OPC_INIT    = 0x18;  // init (preserve config + start)
  static constexpr uint8_t OPC_WR_CONF = 0x80;  // write config + addr
  static constexpr uint8_t OPC_RD_CONF = 0x40;  // read  config + addr
  static constexpr uint8_t OPC_RD_RES  = 0x60;  // read  result  + addr

  // result base-addresses
  static constexpr uint8_t RES_REFID_A3 =  8;
  static constexpr uint8_t RES_TSTOP_A3 = 11;
  static constexpr uint8_t RES_REFID_B3 = 20;
  static constexpr uint8_t RES_TSTOP_B3 = 23;

  // ctor: supply CS pin and SPI instance (user must call SPI.begin before)
  AS6501(uint8_t csPin, SPIClass& spi);

  // must be called once after SPI.begin(...)
  void powerOnReset();
  // preserve config + start conversions
  void initMeasurement();

  // single-register access
  void      writeRegister(uint8_t addr, uint8_t val);
  uint8_t   readRegister( uint8_t addr);

  // bulk config
  void      writeConfig( const uint8_t cfg[NREG]);
  bool      verifyConfig(const uint8_t cfg[NREG]);

  // convenience: switch modes (blocks until config verified)
  bool      configureSingleChannel();
  bool      configureDualChannel();

  // read 24-bit result (REFID or TSTOP)
  uint32_t  readResult(uint8_t baseAddr);

  // status flags
  bool    isPresent();
  bool    isDualMode();

private:
  uint8_t     _cs;
  SPIClass&   _spi;
  SPISettings _settings;
  bool        _present;
  bool        _dualMode;

  // low-level helpers
  void csLow();
  void csHigh();
  void sendOpcode(uint8_t op);

  // configuration maps
  static constexpr uint8_t cfgSingle[NREG] = {
    // CFG0..CFG2
    0x91,            // CFG0: STOPB disabled (bit2=0)
    0x05,            // CFG1: HIT_ENA STOPA/B, independent channels
    0xC0,            // CFG2
    // CFG3..CFG5: REFCLK_DIV = 10000 → 0x002710 → [0x10,0x27,0x00] 10Mhz
    0x10, 0x27, 0x00,
    // CFG6..CFG7
    0x00, 0x00,
    // CFG8..CFG15 = datasheet defaults
    0xA1, 0x13, 0x00, 0x0A,
    0xCC, 0xCC, 0xF1, 0x7D,
    // CFG16 = 0x04 → CMOS_INPUT
    0x04
  };

  static constexpr uint8_t cfgDual[NREG] = {
    // CFG0..CFG2
    0x95,            // CFG0: PIN_ENA STOPA/B + REFCLK + RSTIDX
    0x05,            // CFG1: HIT_ENA STOPA/B, independent channels
    0xC0,            // CFG2
    // CFG3..CFG5: REFCLK_DIV = 10000 → 0x002710 → [0x10,0x27,0x00] 10Mhz
    0x10, 0x27, 0x00,
    // CFG6..CFG7
    0x00, 0x00,
    // CFG8..CFG15 = datasheet defaults
    0xA1, 0x13, 0x00, 0x0A,
    0xCC, 0xCC, 0xF1, 0x7D,
    // CFG16 = 0x04 → CMOS_INPUT
    0x04
  };
};

#endif // AS6501_H
