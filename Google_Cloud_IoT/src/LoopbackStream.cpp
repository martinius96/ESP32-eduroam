#include "LoopbackStream.h"

LoopbackStream::LoopbackStream(uint16_t buffer_size) {
  this->buffer = (uint8_t*) malloc(buffer_size);
  this->buffer_size = buffer_size;
  this->pos = 0;
  this->size = 0;
}
LoopbackStream::~LoopbackStream() {
  free(buffer);
}

void LoopbackStream::clear() {
  this->pos = 0;
  this->size = 0;
}

int LoopbackStream::read() {
  if (size == 0) {
    return -1;
  } else {
    int ret = buffer[pos];
    pos++;
    size--;
    if (pos == buffer_size) {
      pos = 0;
    }
    return ret;
  }
}

size_t LoopbackStream::write(uint8_t v) {
  if (size == buffer_size) {
    return 0;
  } else {
    int p = pos + size;
    if (p >= buffer_size) {
      p -= buffer_size;
    }
    buffer[p] = v;
    size++;
    return 1;
  }  
}

int LoopbackStream::available() {
  return size;
}

int LoopbackStream::availableForWrite() {
  return buffer_size - size;
}

int LoopbackStream::peek() {
  return size == 0 ? -1 : buffer[pos];
}

void LoopbackStream::flush() {
  //I'm not sure what to do here...
}

