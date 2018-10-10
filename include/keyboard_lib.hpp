#pragma once

#include <linux/input.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pthread.h>

class Keyboard
{
private:

    static const int updatingKeyboardStateFrequency = 200;
    static const int updatingKeyboardsListFrequency = 2;
    static const int maxKeysNumber = KEY_CNT;
    static const int maxKeyboarsNumber = 64;
    static const int maxEventsNumber = 1024;
    static const int keyPushSavingDurationMillis = 3000;

    int file;
    bool wasInitialized;
    bool keyDownMap[maxKeysNumber];
    bool keyPushMap[maxKeysNumber];
    boost::posix_time::ptime lastKeyPushTime[maxKeysNumber];
    pthread_t updatingKeyboardStateThread;

    int initKeyboard(unsigned int keyboardNumber);
    int initKeyboardByPath(const char* eventHandler);
    static void* updatingKeyboardState(void* arg);

public:

    Keyboard(unsigned int keyboardNumber = 0);
    Keyboard(const char* eventHandler = "/dev/input/event0");
    bool keyDown(int key);
    bool keyPush(int key);
    ~Keyboard();
};

//If you don`t enable echo after its disabling and the programm finishes to work, echo will be switched off in the console, so you need to restart console to switch on echo again or switch it by the command "stty echo"
void echoEnable(bool enable);

void clearInputBuffer();
