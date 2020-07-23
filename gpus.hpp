#include "config.h"
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server.hpp>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>
#include <xyz/openbmc_project/Inventory/Decorator/Asset/server.hpp>
#include <xyz/openbmc_project/Inventory/Item/server.hpp>
#include <xyz/openbmc_project/Nvme/Status/server.hpp>
#include <xyz/openbmc_project/Sensor/Threshold/Critical/server.hpp>
#include <xyz/openbmc_project/Sensor/Threshold/Warning/server.hpp>
#include <xyz/openbmc_project/Sensor/Value/server.hpp>

namespace phosphor
{
namespace gpu
{

using ValueIface = sdbusplus::xyz::openbmc_project::Sensor::server::Value;

using CriticalInterface =
    sdbusplus::xyz::openbmc_project::Sensor::Threshold::server::Critical;

using WarningInterface =
    sdbusplus::xyz::openbmc_project::Sensor::Threshold::server::Warning;

using GpuIfaces =
    sdbusplus::server::object::object<ValueIface, CriticalInterface, WarningInterface>;

using ItemIface = sdbusplus::xyz::openbmc_project::Inventory::server::Item;

using GpuStatusIfaces =
    sdbusplus::server::object::object<ItemIface>;

class GpuTEMP : public GpuIfaces
{
  public:
    GpuTEMP() = delete;
    GpuTEMP(const GpuTEMP &) = delete;
    GpuTEMP &operator=(const GpuTEMP &) = delete;
    GpuTEMP(GpuTEMP &&) = delete;
    GpuTEMP &operator=(GpuTEMP &&) = delete;
    virtual ~GpuTEMP() = default;

    GpuTEMP(sdbusplus::bus::bus &bus, const char *objPath) :
        GpuIfaces(bus, objPath), bus(bus)
    {
    }

    void setSensorValueToDbus(const u_int64_t value);
    void checkSensorThreshold();
    void setSensorThreshold(uint64_t criticalHigh, uint64_t criticalLow,
                            uint64_t maxValue, uint64_t minValue,
                            uint64_t warningHigh, uint64_t warningLow);

  private:
    sdbusplus::bus::bus &bus;
};

class GpuSTATUS : public GpuStatusIfaces
{
  public:
    GpuSTATUS() = delete;
    GpuSTATUS(const GpuSTATUS &) = delete;
    GpuSTATUS &operator=(const GpuSTATUS &) = delete;
    GpuSTATUS(GpuSTATUS &&) = delete;
    GpuSTATUS &operator=(GpuSTATUS &&) = delete;
    virtual ~GpuSTATUS() = default;

    GpuSTATUS(sdbusplus::bus::bus &bus, const char *objPath) :
        GpuStatusIfaces(bus, objPath), bus(bus)
    {
    }

    void setGpuStatusToDbus(const bool value);

  private:
    sdbusplus::bus::bus &bus;
};
} // namespace gpu
} // namespace phosphor
