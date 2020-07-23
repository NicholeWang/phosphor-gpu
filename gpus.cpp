#include "gpu_manager.hpp"
#include <iostream>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <sstream>
#include <string>


namespace phosphor
{
namespace gpu
{
using namespace std;
using namespace phosphor::logging;

void GpuTEMP::checkSensorThreshold()
{
    uint64_t value = ValueIface::value();
    uint64_t criticalHigh = CriticalInterface::criticalHigh();
    uint64_t criticalLow = CriticalInterface::criticalLow();
    uint64_t warningHigh = WarningInterface::warningHigh();
    uint64_t warningLow = WarningInterface::warningLow();

    if (value > warningHigh)
        WarningInterface::warningAlarmHigh(true);
    else
        WarningInterface::warningAlarmHigh(false);

    if (value < warningLow)
        WarningInterface::warningAlarmLow(true);
    else
        WarningInterface::warningAlarmLow(false);

    if (value > criticalHigh)
        CriticalInterface::criticalAlarmHigh(true);
    else
        CriticalInterface::criticalAlarmHigh(false);

    if (value < criticalLow)
        CriticalInterface::criticalAlarmLow(true);
    else
        CriticalInterface::criticalAlarmLow(false);
}

void GpuTEMP::setSensorThreshold(uint64_t criticalHigh, uint64_t criticalLow,
                                 uint64_t maxValue, uint64_t minValue,
                                 uint64_t warningHigh, uint64_t warningLow)
{

    CriticalInterface::criticalHigh(criticalHigh);
    CriticalInterface::criticalLow(criticalLow);
    ValueIface::maxValue(maxValue);
    ValueIface::minValue(minValue);
    WarningInterface::warningHigh(warningHigh);
    WarningInterface::warningLow(warningLow);
}

void GpuTEMP::setSensorValueToDbus(const u_int64_t value)
{
    ValueIface::value(value);
}
void GpuSTATUS::setGpuStatusToDbus(const bool value)
{
    ItemIface::present(value);
}

} // namespace gpu
} // namespace phosphor
