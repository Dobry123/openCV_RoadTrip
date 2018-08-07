#ifndef COMUNICATION_H
#define COMUNICATION_H

#include <iostream>

class Comunication
{
public:
    int set_interface_attribs (int fd, int speed, int parity);
    void set_blocking (int fd, int should_block);
    int send(int a);
    std::string intToStr(int n);
};

#endif // COMUNICATION_H
