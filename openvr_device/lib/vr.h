#ifndef VR_H
#define VR_H

class VR {
public:
  uint8_t * buffer;
  size_t size;

  VR() {
    this->buffer = new uint8_t[1024];
    memset(this->buffer, 0, 1024);
    this->flush();
  };

  void setDeviceIndex(uint8_t index) {
    this->buffer[0] = (uint8_t) index;
  }

  void setPosition(float x, float y, float z) {
    this->setPositionX(x);
    this->setPositionY(y);
    this->setPositionZ(z);
  }

  void setPositionX(float x) {
    this->buffer[1] = x;
  }

  void setPositionY(float y) {
    this->buffer[5] = y;
  }

  void setPositionZ(float z) {
    this->buffer[9] = z;
  }

  void setRotation(float x, float y, float z, float w) {
    this->setRotationX(x);
    this->setRotationY(y);
    this->setRotationZ(z);
    this->setRotationW(w);
  }

  void setRotationX(float x) {
    memcpy(&(this->buffer[13]), &x, 4);
  }

  void setRotationY(float y) {
    memcpy(&(this->buffer[17]), &y, 4);
  }

  void setRotationZ(float z) {
    memcpy(&(this->buffer[21]), &z, 4);
  }

  void setRotationW(float w) {
    memcpy(&(this->buffer[25]), &w, 4);
  }

  void action(uint8_t action, uint8_t dsize, uint8_t * data) {
    uint8_t byte = 0;
    byte |= action << 4;
    byte |= dsize & 0x0F;

    this->buffer[this->size++] = byte;
    memcpy(&(this->buffer[this->size]), data, dsize);
    this->size += dsize;

    this->buffer[this->size] = (uint8_t) 0;
  }

  void flush() {
    this->size = 29;
    this->buffer[this->size] = (uint8_t) 0;
  }
};

#endif
