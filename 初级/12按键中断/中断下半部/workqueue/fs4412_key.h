#ifndef FS4412_KEY_H
#define FS4412_KEY_H

enum KEYCODE{
    KEY2 = 1002,
    KEY3,
    KEY4
};

enum KEY_STATUS{
    KEY_DOWN = 0,
    KEY_UP,
};

struct keyvalue{
    int code;
    int status;
};

#endif