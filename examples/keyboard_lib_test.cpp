#include <unistd.h>
#include <iostream>
#include <keyboard_lib.hpp>

using std::cout;
using std::cin;
using std::endl;

int main (int argc, char *argv[])
{
    //Keyboard keyboard(0); //Number of keyboard beginning from zero. Performs searching devices with "keyboard" in their discription with the command "lsinput" (needs sudo apt-get install input-utils)
    Keyboard keyboard; //First found keyboard
    //Keyboard keyboard("/dev/input/event14"); //Opening manually the keyboard file, so we don`t need to perform the search (it takes for about 1 second)

    //Use key codes from file "linux/input.h", they are different from ASCII codes

    while (!keyboard.keyPush(KEY_ESC)){
        for(int i = 0; i < KEY_CNT; i++){
            if(keyboard.keyPush(i)) cout << "Pushed key " << i << endl;
	    }

        usleep(50000);
    }
    return 0;
}
