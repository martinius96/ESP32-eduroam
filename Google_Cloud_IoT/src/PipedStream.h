#pragma once

#include "LoopbackStream.h"

/*
 * PipedStreams are created in pairs, and all data written to one of the streams can be read on the other
 * 
 * If any of the internal buffers overflows, the last bytes written are lost.
 * 
 * It can be used as a Serial or Socket-like layer between components.
 */

class PipedStream : public Stream {
  LoopbackStream& in;
  LoopbackStream& out;
public:
  PipedStream(LoopbackStream& in, LoopbackStream& out);

  /** Clear the buffers */
  void clear(); 

  virtual size_t write(uint8_t);
  virtual int availableForWrite(void);
  
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();
};

class PipedStreamPair {
private:
  LoopbackStream buffer1, buffer2;
public:
  PipedStreamPair(uint16_t buffer_size = LoopbackStream::DEFAULT_SIZE);
  PipedStream first, second;
};
