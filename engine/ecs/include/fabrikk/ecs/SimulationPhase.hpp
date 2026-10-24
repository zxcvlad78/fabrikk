#pragma once
#include <cstdint>
#include <string_view>

namespace fabrikk {

enum class SimulationPhase : uint8_t {
    Input,
    FixedUpdate,
    Update,
    Render,
    PostRender,

    Count,
    Invalid
};

constexpr uint8_t kSimulationPhaseCount = static_cast<uint8_t>(SimulationPhase::Count);

inline bool valid(SimulationPhase p) {
    return p != SimulationPhase::Count && p != SimulationPhase::Invalid;
}

inline const char* to_string(SimulationPhase p) {
    switch (p) {
        case SimulationPhase::Input: return "Input";
        case SimulationPhase::FixedUpdate: return "FixedUpdate";
        case SimulationPhase::Update: return "Update";
        case SimulationPhase::Render: return "Render";
        case SimulationPhase::PostRender: return "PostRender";
        default: return "Invalid";
    }
}

inline SimulationPhase from_string(std::string_view s) {
    if (s == "Input") return SimulationPhase::Input;
    if (s == "FixedUpdate") return SimulationPhase::FixedUpdate;
    if (s == "Update") return SimulationPhase::Update;
    if (s == "Render") return SimulationPhase::Render;
    if (s == "PostRender") return SimulationPhase::PostRender;
    return SimulationPhase::Invalid;
}

}