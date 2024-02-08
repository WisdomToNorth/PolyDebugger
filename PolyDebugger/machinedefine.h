#ifndef MACHINEDEFINE_H
#define MACHINEDEFINE_H

enum class MachineType
{
    kCavc,
    kNGPoly,
    kClipper
};

static MachineType g_machineType = MachineType::kCavc;

#endif // GRAPHICSHELPERS_H
