#pragma once

namespace fabrikk {
struct SimulationContext {
    double dt;
    double fixed_dt = 1 / 60.f;
};
}