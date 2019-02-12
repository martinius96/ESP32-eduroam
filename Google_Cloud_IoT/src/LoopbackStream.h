#pragma once

#include <Stream.h>

/*
 * A LoopbackStream stores all data written in an internal buffer and returns it back when the stream is read.
 * 
 * If the buffer overflows, the last bytes written are lost.
 * 
 * It can be used as a buffering layer between components.
 */
class LoopbackStream : public Stream {
  uint8_t *buffer;
  uint16_t buffer_size;
  uint16_t pos, size;
public:
  static const uint16_t DEFAULT_SIZE = 64;
  
  LoopbackStream(uint16_t buffer_size = LoopbackStream::DEFAULT_SIZE);
  ~LoopbackStream();
    
  /** Clear the buffer */
  void clear(); 
  
  virtual size_t write(uint8_t);
  virtual int availableForWrite(void);
  
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();
};
