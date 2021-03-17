#include <Arduino.h>

#include "Capstone_Message.h"

namespace Capstone_Message {
    Message* createTagMessage(size_t size_i, String message) {
        Message* msg = malloc(sizeof(Message));
        msg->intro_size = size_i;
        msg->intro = message;
        msg->content_size = 0;
        msg->content = "";
        return msg;
    }
    
    Message* createBeaconMessage(size_t size_i, String intro_msg, size_t size_c, String content_msg) {
        Message* msg = malloc(sizeof(Message));
        msg->intro_size = size_i;
        msg->intro = intro_msg;
        msg->content_size = size_c;
        msg->content = content_msg;
        return msg;
    }

    void freeMessage(Message* msg) {
      free(&msg->intro);
      free(&msg->content);
      free(msg);
    }
}
