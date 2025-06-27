<h1>AS6501 SPI Interface Library for ESP32</h1> <p> A compact, easy-to-use C++ class for communicating with the AS6501 Time-to-Digital Converter over SPI on Arduino-compatible boards. Handles chip-select toggling, SPI transactions up to 50 MHz, configuration register management, and single/dual-channel timing reads. </p>

<h2>Features</h2> <ul> <li>Power-on reset and measurement initialization sequences</li> <li>Read/write individual config registers or bulk transfer all 17 bytes</li> <li>Automatic verification of configuration with retry loop on mismatch</li> <li>Single-channel (STOPA only) and dual-channel (STOPA + STOPB) modes</li> <li>24-bit result reads for reference (REFID) and stop (TSTOP) timestamps</li> <li>Works with any <code>SPIClass</code> instance (VSPI, HSPI, FSPI on ESP32, etc.)</li> <li>Configurable SPI settings: 50 MHz clock, MSB-first, MODE0</li> </ul>

<h2>Installation</h2> <ol> <li>Copy <code>AS6501_SPI_Class.h</code> and <code>AS6501_SPI_Class.cpp</code> into your project’s <code>libraries/AS6501/</code> folder.</li> <li>Include the header in your sketch: <pre><code>#include &lt;AS6501_SPI_Class.h&gt;</code></pre> </li> <li>Initialize SPI in <code>setup()</code> with <code>SPI.begin()</code> or board-specific variant.</li> </ol>

<h2>Usage</h2>

<h3>1. Instantiate the Class</h3> <pre><code>// Example for ESP32-S3 FSPI on GPIO12=SCK,13=MISO,11=MOSI,10=CS SPIClass fspi(FSPI); fspi.begin(12, 13, 11, 10); // SCK, MISO, MOSI, CS

AS6501 tdc(10, fspi); // CS pin 10, using fspi instance </code></pre>

<h3>2. Power-On and Configure</h3> <pre><code>tdc.powerOnReset(); // Send power-on reset opcode + 5 ms delay tdc.configureDualChannel(); // Or: tdc.configureSingleChannel() Serial.printf( "Present=%d Mode=%s\n", tdc.isPresent(), tdc.isDualMode() ? "dual" : "single" ); </code></pre>

<h3>3. Read Measurements</h3> <pre><code>void loop() { delay(100);

// Channel A uint32_t refA = tdc.readResult(AS6501::RES_REFID_A3); uint32_t stopA = tdc.readResult(AS6501::RES_TSTOP_A3);

// Channel B (only in dual mode) uint32_t refB = tdc.readResult(AS6501::RES_REFID_B3); uint32_t stopB = tdc.readResult(AS6501::RES_TSTOP_B3);

Serial.printf( "A: REF=%lu TSTOP=%lu | B: REF=%lu TSTOP=%lu\n", refA, stopA, refB, stopB ); } </code></pre>

<h2>API Reference</h2> <h3>Constructor</h3> <pre><code>AS6501(uint8_t csPin, SPIClass& spi);</code></pre> <p> <strong>csPin</strong>: GPIO used for chip-select<br> <strong>spi</strong>: Reference to an SPIClass instance </p>

<h3>Status Flags</h3> <ul> <li><code>bool isPresent()</code> — Returns true if configuration verification passed</li> <li><code>bool isDualMode()</code> — Returns true if configured for dual-channel</li> </ul>

<h3>Core Commands</h3> <ul> <li><code>void powerOnReset()</code> — Send power-on reset opcode (0x08) + 5 ms delay</li> <li><code>void initMeasurement()</code> — Send start-conversion opcode (0x0E) + 1 ms delay</li> <li><code>void sendOpcode(uint8_t op)</code> — Send any single-byte opcode</li> </ul>

<h3>Register Operations</h3> <ul> <li><code>void writeRegister(uint8_t addr, uint8_t val)</code></li> <li><code>uint8_t readRegister(uint8_t addr)</code></li> <li><code>void writeConfig(const uint8_t cfg[17])</code> — Bulk write all config bytes</li> <li><code>bool verifyConfig(const uint8_t cfg[17])</code> — Bulk read and compare, updates <code>_present</code></li> </ul>

<h3>Mode Configuration</h3> <ul> <li><code>bool configureSingleChannel()</code> — Configure for STOPA only</li> <li><code>bool configureDualChannel()</code> — Configure for STOPA + STOPB</li> </ul>

<h3>Measurement Readback</h3> <pre><code>uint32_t readResult(uint8_t baseAddr);</code></pre> <p> Sends a 0x50-|baseAddr opcode, then reads three bytes (MSB to LSB) to form a 24-bit value. </p>

<h2>Custom Configuration Arrays</h2> <p> Define two 17-byte arrays <code>cfgSingle[]</code> and <code>cfgDual[]</code> in your header to set thresholds, timing windows, and filter parameters. The library’s default examples assume these are declared and populated in <code>AS6501_SPI_Class.h</code>. </p>

<h2>Example Sketch</h2> <p>See <code>Example.ino</code> in this repo for a full demonstration on initializing FSPI, configuring the chip, and printing timing results.</p>

<h2>References</h2>

<ol>
  <li>
    <a href="https://www.sciosense.com/as6501-time-to-digital-converter/" 
       target="_blank" 
       rel="noopener noreferrer">
      AS6501 Time-to-Digital Converter
    </a>
  </li>
  <li>
    <a href="https://www.sciosense.com/wp-content/uploads/2023/12/AS6501-Datasheet.pdf?_gl=1*1faqjvc*_up*MQ..*_ga*NzY3NzAwMzY5LjE3NTEwNTEzNzc.*_ga_3C0E2J9K40*czE3NTEwNTEzNzYkbzEkZzAkdDE3NTEwNTEzNzYkajYwJGwwJGgw" 
       target="_blank" 
       rel="noopener noreferrer">
      AS6501 Datasheet (PDF)
    </a>
  </li>
</ol>

<h2>Contributing</h2> <ol> <li>Fork the repository</li> <li>Create a feature branch</li> <li>Submit a pull request with clear description and tested code</li> </ol>

<h2>License</h2> <p>MIT © Huzefa T., 2025</p>
