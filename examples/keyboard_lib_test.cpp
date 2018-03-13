#include <keyboard_lib.hpp>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

int main (int argc, char *argv[])
{
    //Keyboard keyboard(0); //Number of keyboard beginning from zero. Performs searching devices with "keyboard" in their discription with the command "lsinput" (needs sudo apt-get install input-utils)
    Keyboard keyboard; //First found keyboard

    //Use key codes from file "linux/input.h", they are different from ASCII codes

    while (!keyboard.keyPush(KEY_ESC)){
        for(int i = 0; i < KEY_CNT; i++){
            if(keyboard.keyPush(i)) cout << "Pushed key " << i << endl;
	    }

        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
    }
    return 0;
}
