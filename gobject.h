#ifndef GOBJECT_H
#define GOBJECT_H

typedef unsigned int uint;

#define ALPHABET_SIZE 256

class GObject {
   protected:
    bool bytes[ALPHABET_SIZE];
    uint bytes_filled;

    void Init() {
        for (int i = 0; i < ALPHABET_SIZE; ++i) bytes[i] = 0;
        bytes_filled = 0;
    }
};

#endif