// Example.ino
// ESP32‐S3 FSPI on GPIO12=SCK,13=MISO,11=MOSI,10=CS
// SPI at 80 MHz (max FSPI)

//----------------------------------------------------------------------------
// Setup FSPI pins for max‐speed SPI
#include <Arduino.h>
#include <SPI.h>
#include "AS6501_SPI_Class.h"

// Create an SPI instance using FSPI
SPIClass spi(FSPI);

void setupFSPI() {
  // FSPI.begin(SCK, MISO, MOSI, SS)
  spi.begin(12 , 13 , 11 , 10 ); //SCK MISO MOSI CS
}

//----------------------------------------------------------------------------
// Instantiate and configure AS6501
AS6501 tdc(10, spi);

void setup() {
  Serial.begin(115200);
  setupFSPI();

  // Power‐on reset
  tdc.powerOnReset();

  // Choose mode; blocks until config verified
  tdc.configureDualChannel();    // or tdc.configureSingleChannel();

  // Report status
  Serial.printf("AS6501 present=%d  mode=%s\n",
                tdc.isPresent(),
                tdc.isDualMode() ? "dual" : "single");
}

void loop() {
  delay(100);
  Serial.printf("AS6501 present=%d  mode=%s\n",
                tdc.isPresent(),
                tdc.isDualMode() ? "dual" : "single");
  // Read channel A
  uint32_t refA  = tdc.readResult(AS6501::RES_REFID_A3);
  uint32_t stopA = tdc.readResult(AS6501::RES_TSTOP_A3);

  // Read channel B (only valid if dual‐mode)
  uint32_t refB  = tdc.readResult(AS6501::RES_REFID_B3);
  uint32_t stopB = tdc.readResult(AS6501::RES_TSTOP_B3);

  Serial.printf("A: REF=%lu TSTOP=%lu | B: REF=%lu TSTOP=%lu\n",
                refA, stopA, refB, stopB);
}