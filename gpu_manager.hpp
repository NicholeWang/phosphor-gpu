#include "config.h"
#include "gpus.hpp"
#include "gpu.hpp"
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server.hpp>
#include <sdbusplus/server/object.hpp>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>

namespace phosphor
{
namespace gpu
{
class Gpu
{
  public:
    Gpu() = delete;
    Gpu(const Gpu &) = delete;
    Gpu &operator=(const Gpu &) = delete;
    Gpu(Gpu &&) = delete;
    Gpu &operator=(Gpu &&) = delete;

    Gpu(sdbusplus::bus::bus &bus, const char *objPath) : bus(bus), _event(sdeventplus::Event::get_default()), _timer(_event, std::bind(&Gpu::read, this)), _objPath(objPath)
    {
    }

    struct GPUConfig
    {
        uint8_t index;
        uint8_t busID;
        uint8_t address;
        uint8_t channel;
        uint64_t criticalHigh;
        uint64_t criticalLow;
        uint64_t maxValue;
        uint64_t minValue;
        uint64_t warningHigh;
        uint64_t warningLow;
    };

    struct GPUData {
      u_int64_t sensorValue;/* Sensor value, if sensor value didn't be update, means sensor failure,
                               default set to 168 accroding to Nvidia GPU SPEC*/
    };

    void run();
    const std::string _objPath;

    std::string getValue(std::string);
    std::unordered_map<std::string, std::shared_ptr<phosphor::gpu::GpuTEMP>>
        gpus;
    std::unordered_map<std::string, std::shared_ptr<phosphor::gpu::GpuSTATUS>>
        gpustatus;

  private:
    sdbusplus::bus::bus &bus;

    sdeventplus::Event _event;
    /** @brief Read Timer */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> _timer;

    void init();
    void read();

};
} // namespace gpu
} // namespace phosphor
