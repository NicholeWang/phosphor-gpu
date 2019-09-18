#include "gpu_manager.hpp"
#include "gpu.hpp"
#include "nlohmann/json.hpp"
//#include "smbus.hpp"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <sstream>
#include <string>
#include "i2c-dev.h"

#define MAX_I2C_BUS 14
#define MONITOR_INTERVAL_SENCODS 1

static int fd[MAX_I2C_BUS] = {0};
static constexpr auto configFile = "/etc/gpu/gpu_config.json";
auto retries = 3;
static constexpr auto delay = std::chrono::milliseconds{100};
using Json = nlohmann::json;
std::vector<phosphor::gpu::Gpu::GPUConfig> configs;
namespace fs = std::experimental::filesystem;

namespace phosphor
{
namespace gpu
{

using namespace std;
using namespace phosphor::logging;

void Gpu::run()
{
    init();

    std::function<void()> callback(std::bind(&Gpu::read, this));
    try
    {
        u_int64_t interval = MONITOR_INTERVAL_SENCODS * 1000000;
        _timer.restart(std::chrono::microseconds(interval));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in polling loop. ERROR = " << e.what() << std::endl;
    }
}

Json parseSensorConfig()
{
    std::ifstream jsonFile(configFile);
    if (!jsonFile.is_open())
    {
        std::cerr << "GPU config JSON file not found" << std::endl;
    }

    auto data = Json::parse(jsonFile, nullptr, false);
    if (data.is_discarded())
    {
        std::cerr << "GPU config readings JSON parser failure" << std::endl;
    }

    return data;
}

std::vector<phosphor::gpu::Gpu::GPUConfig> getGpuConfig()
{

    phosphor::gpu::Gpu::GPUConfig gpuConfig;
    std::vector<phosphor::gpu::Gpu::GPUConfig> gpuConfigs;
    uint64_t criticalHigh = 0;
    uint64_t criticalLow = 0;
    uint64_t maxValue = 0;
    uint64_t minValue = 0;
	uint64_t warningHigh = 0;
    uint64_t warningLow = 0;

    try
    {
        auto data = parseSensorConfig();
        static const std::vector<Json> empty{};
        std::vector<Json> readings = data.value("config", empty);
        std::vector<Json> thresholds = data.value("threshold", empty);
        if (!thresholds.empty())
        {
            for (const auto &instance : thresholds)
            {
                criticalHigh = instance.value("criticalHigh", 0);
                criticalLow = instance.value("criticalLow", 0);
                maxValue = instance.value("maxValue", 0);
                minValue = instance.value("minValue", 0);
				warningHigh = instance.value("warningHigh", 0);
                warningLow = instance.value("warningLow", 0);
            }
        }
        else
        {
            std::cerr << "Invalid GPU config file, thresholds dosen't exist" << std::endl;
        }

        if (!readings.empty())
        {
            for (const auto &instance : readings)
            {
                uint8_t index = instance.value("GPUIndex", 0);
                uint8_t busID = instance.value("GPUBusID", 0);
                uint8_t address = instance.value("GPUAddress", 0);
                uint8_t channel = instance.value("GPUChannel", 0);

                gpuConfig.index = index;   
                gpuConfig.busID = busID;
                gpuConfig.address = address;
				gpuConfig.channel = channel;
                gpuConfig.criticalHigh = criticalHigh;
                gpuConfig.criticalLow = criticalLow;
                gpuConfig.maxValue = maxValue;
                gpuConfig.minValue = minValue;
				gpuConfig.warningHigh = warningHigh;
                gpuConfig.warningLow = warningLow;
                gpuConfigs.push_back(gpuConfig);
            }
        }
        else
        {
            std::cerr << "Invalid GPU config file, config dosen't exist" << std::endl;
        }
    }
    catch (const Json::exception &e)
    {
        std::cerr << "Json Exception caught. MSG: " << e.what() << std::endl;
    }

    return gpuConfigs;
}

void Gpu::init()
{
    // read json file
    configs = getGpuConfig();
}

void Gpu::read()
{
	
    for (int i = 0; i < configs.size(); i++)
    {
        GPUData gpuData;
		int Value = 0;	
        auto iter = gpus.find(std::to_string(configs[i].index));
        
        std::cerr << "GPU index = "<< i << std::endl;

		// get GPU information through i2c by busID.
        auto success = getGPUInfobyBusID(configs[i].busID, configs[i].address, configs[i].channel, &Value); 
		gpuData.sensorValue = (u_int64_t)Value;
					
        // can not find. create dbus
        if (iter == gpus.end())
        {
             std::string objPath = 
                 GPU_OBJ_PATH + std::to_string(configs[i].index);

             auto gpuTEMP = std::make_shared<phosphor::gpu::GpuTEMP>(
                 bus, objPath.c_str());
             gpus.emplace(std::to_string(configs[i].index), gpuTEMP);

             gpuTEMP->setSensorValueToDbus(gpuData.sensorValue);
             gpuTEMP->setSensorThreshold(
                 configs[i].criticalHigh, configs[i].criticalLow,
                 configs[i].maxValue, configs[i].minValue,
                 configs[i].warningHigh, configs[i].warningLow);

             gpuTEMP->checkSensorThreshold();
         }
         else
         {
             iter->second->setSensorValueToDbus(gpuData.sensorValue);
             iter->second->checkSensorThreshold();
         }

		 if (0 == success)
		 {
		     gpus.erase(std::to_string(configs[i].index));
			 continue;
		 }
			 
    }
}
} // namespace gpu
} // namespace phosphor
