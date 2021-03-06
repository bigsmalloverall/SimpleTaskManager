/*
* BliMultiple tasksnk example for Simple Task Manager
* 
* Header file declaring all needed variables and functions.
* 
* by Jakub Wójcik 2021
* jakub.wojcik2017@gmail.com
* 
* https://github.com/bigsmalloverall/SimpleTaskManager
*/

#ifndef ARDUINO_TASKER_SERIAL_LOOP_TASK
#define ARDUINO_TASKER_SERIAL_LOOP_TASK

#include <Arduino.h>
#include <Task.h>

class SerialLoopTask : public SimpleTM::Task
{
private:

    uint32_t _loops {100000}; // how many loops to do before serial print
    uint32_t _currentLoop {0}; 

public:
    SerialLoopTask(uint16_t id);

    // Update is called every loop from main.cpp
    // deltaT is time in us from last time update() was called
    void update(uint32_t deltaT);
};

#endif