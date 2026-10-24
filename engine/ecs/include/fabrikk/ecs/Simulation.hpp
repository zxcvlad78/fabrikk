#pragma once

#include <fabrikk/core/types.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "SimulationPhase.hpp"

namespace fabrikk {
class Simulation {
public:
    struct SystemEntry { std::string id; int priority; std::function<void()> func; };
    struct SystemPhaseBucket { std::vector<SystemEntry> entries; short entries_count; bool sorted = true; };

    std::array<SystemPhaseBucket, kSimulationPhaseCount> _buckets;
    SystemPhaseBucket& bucket(SimulationPhase phase);

    void sort(SimulationPhase phase);
private:
    std::vector<SystemEntry> systems_;
    u64 tick_count_ = 0;
    entt::registry reg_;
public:
    void emplace(SimulationPhase phase, std::function<void()> func, const std::string& id, int priority = 0);
    void clear();
    void clear(SimulationPhase phase);

    void remove(const std::string& id);
    void remove(const std::string& id, SimulationPhase phase);

    bool has(const std::string& id);
    bool has(const std::string& id, SimulationPhase phase);

    void run(SimulationPhase phase);

    std::vector<std::string> list();
    std::vector<std::string> list(SimulationPhase phase);
    
    u64 tick_count() const;
    void tick_next();

    entt::registry& registry();
};
}
