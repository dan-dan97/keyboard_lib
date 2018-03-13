#include <keyboard_lib.hpp>
#include <sys/file.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <limits>
#include <termios.h>
#include <errno.h>

int Keyboard::initKeyboard(const char* eventHandler)
{
    for(int i = 0; i < maxKeysNumber; i++) {
        keyDownMap[i] = 0;
        keyPushMap[i] = 0;
    }

    if(!eventHandler) return -1;

    file = open(eventHandler, O_RDONLY|O_NONBLOCK);
    if (file == -1){
        std::cout << "Error opening keyboard file! Maybe you need unroot it" << std::endl;
        return -1;
    }

    if(!wasInitialized){
        wasInitialized = 1;
        pthread_create(&updatingKeyboardStateThread, NULL, updatingKeyboardState, (void*)this);
    }
    return 0;
}

int Keyboard::initKeyboard(int keyboardNumber)
{
    const int bufferSize = 1024;
    static std::string filesNames[maxKeyboarsNumber];
    char buffer[bufferSize];
    FILE *commandFile;
    int keyboardsNumber = 0;
    unsigned long findRes1, findRes2;
    bool deviceNameIsGood, deviceEventsAreGood;

    if ((commandFile = popen("lsinput", "r")) != NULL) {
        while(keyboardsNumber < maxKeyboarsNumber) {
            if(fgets(buffer, bufferSize, commandFile) == NULL)
                break;
            filesNames[keyboardsNumber] = buffer;
            filesNames[keyboardsNumber].erase(filesNames[keyboardsNumber].length() - 1, 1);
            deviceNameIsGood = 0;
            deviceEventsAreGood = 0;
            while(1){
                fgets(buffer, bufferSize, commandFile);
                if(buffer[0] == 10 && buffer[1] == 0) break;
                std::string str = buffer;
                findRes1 = str.find("name");
                findRes2 = str.find(':');
                if(findRes1 != std::string::npos && findRes2 != std::string::npos && findRes1 < findRes2)
                {
                    str.erase(0, str.find('\"') + 1);
                    str.erase(str.find('\"'), 2);
                    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
                    //boost::algorithm::to_lower(str);
                    if(str.find("keyboard") != std::string::npos){
                        deviceNameIsGood = 1;
                        continue;
                    }
                }
                findRes1 = str.find("bits ev");
                findRes2 = str.find(':');
                if(findRes1 != std::string::npos && findRes2 != std::string::npos && findRes1 < findRes2)
                    if(str.find("EV_SYN") != std::string::npos && str.find("EV_KEY") != std::string::npos){
                        deviceEventsAreGood = 1;
                        continue;
                    }
            }
            if(deviceNameIsGood && deviceEventsAreGood)keyboardsNumber++;
        }
        if(keyboardNumber < 0 || (keyboardNumber >= keyboardsNumber && keyboardsNumber != 0))
        {
            if(!wasInitialized)std::cout << "Invalid keyboard number!" << std::endl;
            keyboardNumber = -1;
        }
        if(keyboardsNumber == 0)
        {
            if(!wasInitialized)std::cout << "Error reading keyboars list! Maybe you need root access or input-utils is not installed" << std::endl;
            keyboardNumber = -1;
        }
    }
    else {
        std::cout << "Error running lsinput" << std::endl;
        keyboardNumber = -1;
    }
    pclose(commandFile);

    return initKeyboard(keyboardNumber == -1 ? NULL : filesNames[keyboardNumber].c_str());
}

void* Keyboard::updatingKeyboardState(void* arg)
{
    Keyboard& keyboard = *(Keyboard*)arg;
    struct input_event events[maxEventsNumber];
    int bytesRead;
    bool deviceConnected = 1;

    while(true) {
        boost::posix_time::ptime iterationTime = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration delay_duration;

        if(deviceConnected)
        {
            bytesRead = read(keyboard.file, (void *)&events, sizeof(events));
            if(errno == ENODEV){
                close(keyboard.file);
                for(int i = 0; i < maxKeysNumber; i++) {
                    keyboard.keyDownMap[i] = 0;
                    keyboard.keyPushMap[i] = 0;
                }
                deviceConnected = 0;
                continue;
            }

            int eventsNumber = bytesRead / (int) sizeof(input_event);
            for (int i = 0; i < eventsNumber; i++){
                if (events[i].type == EV_KEY && events[i].code >= 0 && events[i].code < maxKeysNumber) {
                    if(keyboard.keyDownMap[events[i].code] == 0 && events[i].value == 1)
                        keyboard.keyPushMap[events[i].code] = 1;
                    keyboard.keyDownMap[events[i].code] = events[i].value;
                    if(keyboard.keyDownMap[events[i].code] == 1 && keyboard.keyPushMap[events[i].code] == 1)
                        keyboard.lastKeyPushTime[events[i].code] = boost::posix_time::microsec_clock::local_time();
                }
            }
            delay_duration = boost::posix_time::milliseconds(1.0 / updatingKeyboardStateFrequency * 1000);
        }
        else {
            if(!keyboard.initKeyboard(0)) {
                deviceConnected = 1;
                delay_duration = boost::posix_time::milliseconds(1.0 / updatingKeyboardStateFrequency * 1000);
            }
            else
                delay_duration = boost::posix_time::milliseconds(1.0 / updatingKeyboardsListFrequency * 1000);
        }
        delay_duration -= (boost::posix_time::microsec_clock::local_time() - iterationTime);
        if(delay_duration.total_milliseconds() > 0)
            boost::this_thread::sleep(delay_duration);
    }
}

Keyboard::Keyboard(const char* eventHandler)
{
    wasInitialized = 0;
    echoEnable(0);
    initKeyboard(eventHandler);
}

Keyboard::Keyboard(int keyboardNumber)
{
    wasInitialized = 0;
    echoEnable(0);
    initKeyboard(keyboardNumber);
}

bool Keyboard::keyDown(int key)
{
    if(key >= 0 && key < maxKeysNumber)
        return keyDownMap[key];
    else return 0;
}

bool Keyboard::keyPush(int key)
{
    if(key >= 0 && key < maxKeysNumber) {
        bool res = keyPushMap[key];
        keyPushMap[key] = 0;
        if(res == 1 && (boost::posix_time::microsec_clock::local_time() - lastKeyPushTime[key]).total_milliseconds() > keyPushSavingDurationMillis)
            res = 0;
        return res;
    }
    else return 0;
}

Keyboard::~Keyboard()
{
	std::cout << "Ending keyboard" << std::endl;
    pthread_cancel(updatingKeyboardStateThread);
    pthread_join(updatingKeyboardStateThread, NULL);
    close(file);

	std::cout << "1" << std::endl;
    clearInputBuffer();
	std::cout << "2" << std::endl;

    //Enabling echo in console
    echoEnable(1);
	std::cout << "Ended keyboard" << std::endl;
}

void echoEnable(bool enable)
{
    static int echoOn = -1;
    if(echoOn == -1 || echoOn != enable){
	echoOn = enable;
        struct termios TermConf;
        tcgetattr(STDIN_FILENO, &TermConf);

        if(echoOn)
            TermConf.c_lflag |= (ICANON | ECHO);
        else
            TermConf.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &TermConf);
    }
}

void* stdinClearFunction(void* arg){
    std::cin.ignore();
}

void clearInputBuffer()
{
    pthread_t stdinClearThread;
    pthread_create(&stdinClearThread, NULL, stdinClearFunction, NULL);
    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    pthread_cancel(stdinClearThread);
    pthread_join(stdinClearThread, NULL);
}
