#include <iostream>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include "serial_moter.hpp"

serial_moter::serial_moter(std::string port){
    // Open the serial port
    serialOutput = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
    if(serialOutput == -1)  {
        std::cerr << "Could not open serial port " << port << ": " << strerror(errno) << std::endl;
        if(errno == EACCES)
             std::cerr << "Run 'sudo chmod a+rw "<< port << "' in a terminal to gain access to the port.";
        std::cout << "The execution failed. Press ENTER to continue.";
        std::cin.get(); // Pause the system
        return;
    }

    // Get control settings
    int result;
    struct termios settings;

    result = tcgetattr(serialOutput, &settings);
    if(result == -1) {
        std::cerr << "Could not get settings from open port: " << strerror(errno) << std::endl;
        std::cout << "The execution failed. Press ENTER to continue.";
        std::cin.get(); // Pause the system
        return;
    }

    // Set serial_moter serial settings
    settings.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    settings.c_iflag = IGNPAR;
    settings.c_oflag = 0;
    settings.c_lflag = 0;
    settings.c_cc[VMIN]  = 0;
    settings.c_cc[VTIME] = 0;

    result = tcsetattr(serialOutput, TCSANOW, &settings);
    if(result == -1){
        std::cerr << "Could not set new settings: " << strerror(errno) << std::endl;
        std::cout << "The execution failed. Press ENTER to continue.";
        std::cin.get(); // Pause the system
        return;
    }
}

serial_moter::~serial_moter(){
    close(serialOutput);
}

void serial_moter::moveAngle(int angle){
//     Angle to 90:9000=0x2328
//     3E A3 01 08 EA 28 23 00 00 00 00 00 00 4B
//     Angel to 0:0000
//     3E A3 01 08 EA 00 00 00 00 00 00 00 00 00
    std::vector<uint8_t> commands = std::vector<uint8_t>();
    commands.push_back(0x3E);
    commands.push_back(0xA3);
    commands.push_back(0x01);
    commands.push_back(0x08);
    commands.push_back(0xEA);
    int highByte = (angle*100) / 256;
    int lowByte = (angle*100) % 256;
    int checkSum = highByte + lowByte;
    commands.push_back(lowByte);
    commands.push_back(highByte);
    commands.push_back(0x00);
    commands.push_back(0x00);
    commands.push_back(0x00);
    commands.push_back(0x00);
    commands.push_back(0x00);
    commands.push_back(0x00);
    commands.push_back(checkSum);
    std::cout << "angle: " << angle 
              <<std::hex<< ";highByte: " << highByte 
              <<";lowByte: " << lowByte 
              <<std::dec<<" " ;
//               << std::endl; 
    sendCommands(commands);
}

void serial_moter::openloop(int power){
//     open loop to nearist:
//     3E A0 01 02 E1 00 01 01
    std::vector<uint8_t> commands = std::vector<uint8_t>();
    commands.push_back(0x3E);//head
    commands.push_back(0xA0);//cmd
    commands.push_back(0x01);//Id
    commands.push_back(0x02);//data lenght
    commands.push_back(0xE1);//data CRC
    int highByte = (power*10) / 256;
    int lowByte = (power*10) % 256;
    int checkSum = highByte + lowByte;
    commands.push_back(lowByte);
    commands.push_back(highByte);
    commands.push_back(checkSum);
    sendCommands(commands);
}

void serial_moter::openloop_demo(){
//     open loop to nearist:
//     3E A0 01 02 E1 00 01 01
    std::vector<uint8_t> commands = std::vector<uint8_t>();
    commands.push_back(0x3E);
    commands.push_back(0xA0);
    commands.push_back(0x01);
    commands.push_back(0x02);
    commands.push_back(0xE1);
    commands.push_back(0x00);
    commands.push_back(0x01);
    commands.push_back(0x01);
    sendCommands(commands);
}

void serial_moter::openloop_zero(){
//     open loop to nearist:
//     3E A0 01 02 E1 00 00 00
    std::vector<uint8_t> commands = std::vector<uint8_t>();
    commands.push_back(0x3E);
    commands.push_back(0xA0);
    commands.push_back(0x01);
    commands.push_back(0x02);
    commands.push_back(0xE1);
    commands.push_back(0x00);
    commands.push_back(0x00);
    commands.push_back(0x00);
    sendCommands(commands);
}

void serial_moter::sendCommands(std::vector<uint8_t> commands){
    // Receives a series of commands and sends it to the robot.
    int result = write(serialOutput, &commands[0], commands.size());
    if(result == -1) {
        std::cerr << "Could not write to serial output: " << strerror(errno) << std::endl;
    }
}
