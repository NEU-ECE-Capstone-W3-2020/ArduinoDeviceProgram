#ifndef MESSAGE_H
#define MESSAGE_H

#include "Arduino.h";

typedef struct Message {
  size_t intro_size;
  String intro;
  size_t content_size;
  String content;
} Message;

namespace Capstone_Message {
	Message* createTagMessage(size_t size, String message);
  Message* createBeaconMessage(size_t size_i, String intro_msg, size_t size_c, String content_msg);
  void freeMessage(Message* msg);
}

#endif
