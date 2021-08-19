#include "gpu.hpp"
#include "smbus.hpp"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>  
#include "i2c-dev.h"
#define STR_SIZE 16
#define GPU_SLAVE_ADDRESS 24 //0x18
// int num_list[8];
using namespace std;

char bin_str[STR_SIZE];

long hex_tmp;   
int TEMP;
void HexToBinStr(int hex, char *bin_str, int str_size)
{
    int i;
    for (i = 0; i !=str_size; ++i)
    {
        bin_str[str_size - 1 - i] = hex % 2;
        hex /= 2;
    }
   
    
}

bool getGPUInfobyBusID(int busID, int address, int channel, int* value)
{
    phosphor::smbus::Smbus smbus;
    unsigned char rsp_data_command[8] = {0};
    unsigned char command = 5; // command code
    int num[9];
    int res3_int = num[0];
    memset(num,0, sizeof(num));
    auto init = smbus.smbusInit(busID);
    if (init == -1)
    {
        std::cerr << "smbusInit fail!" << std::endl;
        return false;
    }
   cout << "channel: "<<channel<<endl;
   //Set channel
   auto res_int = smbus.SetSmbusCmdByte(busID, address, 0, channel);
   if (res_int < 0)
   {
       std::cerr << "SET GPU bus:" << busID << " address:" << address << " channel:" << channel << " Fail!" << std::endl;
       smbus.smbusClose(busID);
       return false;
   }

   //Get data
    for(int j=0; j<8; j++)
    {
        num[j]=smbus.GetSmbusCmdWord(busID, GPU_SLAVE_ADDRESS+j, command);
        cout << "2-1 Test: " << num[j]  <<endl;
    }
    
    for (int i=0; i<8; i++) 
    {
        if (num[i] > res3_int)
            res3_int = num[i];
    }
   HexToBinStr(res3_int,bin_str,STR_SIZE);
   TEMP=bin_str[0]*8+bin_str[1]*4+bin_str[2]*2+bin_str[3]+bin_str[12]*128+bin_str[13]*64+bin_str[14]*32+bin_str[15]*16;
   cout << "2-2 Test: " << res3_int  <<endl;
   cout << "2-3 Test: " << TEMP  <<endl;
   if (res3_int < 0)
   {
       std::cerr << "GET GPU bus:" << busID << " address:" << GPU_SLAVE_ADDRESS << " Fail!" << std::endl;
       smbus.smbusClose(busID);
       return false;
   }
   //close channel
   int res2_int = smbus.SetSmbusCmdByte(busID, address, 0, 0);
   if (res2_int < 0)
   {
       smbus.smbusClose(busID);
   }
    *value = (int)TEMP;
   cout << *value <<endl;
    smbus.smbusClose(busID);
    return true;
}
