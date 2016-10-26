#ifndef ERIW1_H
#define ERIW1_H

class Eriw1{

public:
  Eriw1(uint8_t);

  uint8_t reset();

  void write(uint8_t);
  uint8_t read();

  uint8_t crc8(uint8_t [], uint8_t = 8);
  void crcbit(uint8_t);

  void search_init();
  uint8_t search();

  void skip();
  void readROM(uint8_t []);

  uint8_t addr[8];
  uint8_t crc;

private:

  uint8_t readbit();
  void writebit(uint8_t);

  uint8_t pin;

  int8_t tolastprev;
};

#endif
