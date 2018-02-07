#ifndef KEYBOARD_LIB_HPP
#define KEYBOARD_LIB_HPP

#include <linux/input.h>
#include <boost/date_time/posix_time/posix_time.hpp>

class Keyboard
{
private:

    static const int updatingKeyboardStateFrequency = 200;
    static const int updatingKeyboardsListFrequency = 2;
    static const int maxKeysNumber = KEY_CNT;
    static const int maxKeyboarsNumber = 64;
    static const int maxEventsNumber = 1024;
    static const int keyPushSavingDurationMillis = 1000;

    int file;
    bool wasInitialized;
    bool keyDownMap[maxKeysNumber];
    bool keyPushMap[maxKeysNumber];
    boost::posix_time::ptime lastKeyPushTime[maxKeysNumber];
    pthread_t updatingKeyboardStateThread;

    int initKeyboard(const char* eventHandler);
    int initKeyboard(int keyboardNumber);
    static void* updatingKeyboardState(void* arg);

public:

    Keyboard(const char* eventHandler);
    Keyboard(int keyboardNumber = 0);
    bool keyDown(int key);
    bool keyPush(int key);
    ~Keyboard();
};

//If you don`t enable echo after its disabling and the programm finishes to work, echo will be switched off in the console, so you need to restart console to switch on echo again or switch it by the command "stty echo"
void echoEnable(bool enable);

void clearInputBuffer();

#endif //KEYBOARD_LIB_HPP
