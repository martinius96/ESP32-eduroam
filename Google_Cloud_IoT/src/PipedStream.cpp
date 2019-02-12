#include "PipedStream.h"

PipedStream::PipedStream(LoopbackStream& in, LoopbackStream& out) 
: in(in), out(out) 
{ }


void PipedStream::clear() {
  return in.clear();
  return out.clear();
}

int PipedStream::read() {
  return in.read();
}

size_t PipedStream::write(uint8_t v) {
  return out.write(v);
}

int PipedStream::available() {
  return in.available();
}

int PipedStream::availableForWrite() {
  return out.available();
}

int PipedStream::peek() {
  return in.peek();
}

void PipedStream::flush() {
  out.flush();
}

PipedStreamPair::PipedStreamPair(uint16_t buffer_size)
: buffer1(buffer_size),
  buffer2(buffer_size),
  first(buffer1, buffer2),
  second(buffer2, buffer1)
{ }
