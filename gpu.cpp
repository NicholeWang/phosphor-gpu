#include "gpu.hpp"
#include "smbus.hpp"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <sstream>
#include <string>
#include "i2c-dev.h"

#define GPU_SLAVE_ADDRESS 24 //0x18
using namespace std;
bool getGPUInfobyBusID(int busID, int address, int channel, int value)
{
    phosphor::smbus::Smbus smbus;
    unsigned char rsp_data_command[8] = {0};
    unsigned char command = 5; // command code

    auto init = smbus.smbusInit(busID);
    if (init == -1)
    {
        std::cerr << "smbusInit fail!" << std::endl;
        return false;
    }
   cout << "1 Test: " <<endl;
   //Set channel
   auto res_int = smbus.SetSmbusCmdByte(busID, address, 0, channel);
   if (res_int < 0)
   {
	   cout << "10 Test: " <<endl;
       std::cerr << "SET GPU bus:" << busID << " address:" << address << " channel:" << channel << " Fail!" << std::endl;
       smbus.smbusClose(busID);
       return false;
   }
   cout << "2 Test: " <<endl;
   //Get data
   res_int = smbus.GetSmbusCmdByte(busID, GPU_SLAVE_ADDRESS, command);
   if (res_int < 0)
   {
	   cout << "11 Test: " <<endl;
       std::cerr << "GET GPU bus:" << busID << " address:" << GPU_SLAVE_ADDRESS << " Fail!" << std::endl;
       smbus.smbusClose(busID);
       return false;
   }
   cout << "3 Test: " <<endl;
   //close channel
   auto res2_int = smbus.SetSmbusCmdByte(busID, address, 0, 0);
   if (res2_int < 0)
   {
       smbus.smbusClose(busID);
   }
   cout << "4 Test: " <<endl;
   cout << value <<endl;
   value = (int)res_int;
    smbus.smbusClose(busID);
    return true;
}
