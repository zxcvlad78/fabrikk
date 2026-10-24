#pragma once

namespace fabrikk::ctx {
struct SimulationContext {
    double dt;
    double fixed_dt = 1 / 60.f;
};
}