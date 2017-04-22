#include <Arduino.h>
#include "Eriw1.h"

Eriw1::Eriw1(uint8_t _pin){
  pin=_pin;
  digitalWrite(pin,0);
  pinMode(pin,INPUT);
}

uint8_t Eriw1::reset(){
  uint8_t i;

  pinMode(pin,OUTPUT);
  delayMicroseconds(640);
  pinMode(pin,INPUT);
  delayMicroseconds(2);
  for (i=80; i; i--){
    if (digitalRead(pin)==0){
      for (i=120; i; i--){
        if (digitalRead(pin)==1) return 1;
        delayMicroseconds(2);
      }
      return 0;
    }
    delayMicroseconds(2);
  }
  return 0;
}

uint8_t Eriw1::readbit(){
  uint8_t r;
  pinMode(pin,OUTPUT);
  delayMicroseconds(2);
  pinMode(pin,INPUT);
  delayMicroseconds(8);
  r = digitalRead(pin);
  delayMicroseconds(60);
  return r;
}

uint8_t Eriw1::read(){
  uint8_t i, r, b;

  for (r=i=0; i<8; i++){
    crcbit(b=readbit());
    r |= (b<<i);
  }
  return r;
}

void Eriw1::writebit(uint8_t b){
  static uint8_t times[] = { 70, 2};
  b = (b&1);
  pinMode(pin,OUTPUT);
  delayMicroseconds(times[b]);
  pinMode(pin,INPUT);
  delayMicroseconds(times[1-b]);
}

void Eriw1::write(uint8_t b){
  uint8_t i;
  for (i=0; i<8; i++)
    writebit(b>>i);
}

void Eriw1::search_init(){
  for (uint8_t i=0; i<8 ;i++)
    addr[i]=0;
  tolastprev=-1;
}

uint8_t Eriw1::search(){
  int8_t lastdef=64;
  uint8_t d;

  if (tolastprev==64) return 0;

  reset();
  write(0xF0);
  crc=0;

  for (uint8_t i=0; i<64; i++) {
    uint8_t ibit= 1<<(i&7);
    uint8_t ibyte= i>>3;
    if (readbit())
      if (readbit())
        if ((i==tolastprev) || (i<tolastprev && (addr[ibyte]&ibit)==0)) d=0;
        else                                                 lastdef=i, d=1;
      else                                                              d=1;
    else
      if (readbit())                                                    d=0;
      else {
        tolastprev=64;
        return 0; //no device
      }

    crcbit(d);
    writebit(d);
    if (d) addr[ibyte] |= ibit;
    else   addr[ibyte] &= ~ibit;
  }

  tolastprev=lastdef;
  return 1;
}

void Eriw1::crcbit(uint8_t b){
  crc = ((crc ^ b) & 1) ? (crc>>1) ^ 0b10001100 : crc>>1;
}

uint8_t Eriw1::crc8(uint8_t addr[], uint8_t len){
  uint8_t i, j, b;

  for (crc=i=0; i<len; i++)
    for (j = 0, b = addr[i]; j<8; j++, b >>=1)
      crcbit(b);

  return crc;
}

inline void Eriw1::skip(){
  write(0xCC);
}

void Eriw1::readROM(uint8_t addr[]){
  int i;
  write(0x33);
  crc=0;
  for(i=0; i<8; i++)
    addr[i]=read();
}
