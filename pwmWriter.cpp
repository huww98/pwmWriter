#include <wiringPi.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono_literals;

const char* gpioDevPath = "/dev/gpiopwm";

void* writePwm(void* data)
{
    int pin = *static_cast<int*>(data);

    stringstream pathss;
    pathss << gpioDevPath << "/" << pin;
    auto path = pathss.str();

    if((mkfifo(path.c_str(), 0666) < 0) && (errno != EEXIST))
    {
        cerr << "cannot create fifo " << path << endl;
        exit(1);
    }

    cout << "listening on:" << path << endl;

    while(true)
    {
        ifstream ifs(path);
        int pwm;
        while(ifs >> pwm)
        {
            timeval tv;
            gettimeofday(&tv, nullptr);
            cout << "pin: " << pin << " pwm: "<< pwm << " time: " << tv.tv_sec << "."<< tv.tv_usec << endl;
            pwmWrite(pin, pwm);
        }
    }

    return nullptr;
}

int main(int argc, char** argv)
{
    this_thread::sleep_for(30s); // I don't know why..
    umask(0);
    if(mkdir(gpioDevPath,0755) == -1 && errno != EEXIST)
    {
        cerr << "cannot create dir "<< gpioDevPath << endl;
        return 1;
    }

    auto dirp = opendir(gpioDevPath);
    struct dirent* ep;

    vector<int> pwmPins;

    if(argc == 1)
    {
        cout << "you should specify pin numbers from arguments." << endl;
    }
    for(int i = 1; i < argc; i++)
    {
        stringstream ss(argv[i]);
        int pin;
        if(!(ss >> pin) || pin < 0 || pin > 63)
        {
            cerr << "arguments should be pin numbers" << endl;
            return 1;
        }
        pwmPins.push_back(pin);
    }

    wiringPiSetupGpio();

    vector<pthread_t> threads;

    for (int& pin : pwmPins)
    {
        pinMode(pin, PWM_OUTPUT);
        pthread_t newThread;
        pthread_create(&newThread,nullptr,writePwm, &pin);
        threads.push_back(newThread);
    }

    for(auto& t : threads)
    {
        pthread_join(t, nullptr);
    }

    return 0;
}
