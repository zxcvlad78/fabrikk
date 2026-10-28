#pragma once

#include <fabrikk/core/types.hpp>
#include <entt/entt.hpp>
#include <array>
#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "SimulationPhase.hpp"

namespace fabrikk {

class Simulation {
public:
    struct SystemEntry {
        std::string id;
        std::function<void(entt::registry&)> func;
        int priority = 0;
    };

    struct SystemPhaseBucket {
        std::vector<SystemEntry> entries;
        bool sorted = true;
    };

    struct AutoSystemEntry {
        SimulationPhase phase;
        SystemEntry entry;
    };

    Simulation();
    ~Simulation() = default;

    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;

    void emplace(SimulationPhase phase, SystemEntry entry);
    void clear();
    void clear(SimulationPhase phase);

    void remove(const std::string& id);
    void remove(const std::string& id, SimulationPhase phase);

    [[nodiscard]] bool has(const std::string& id) const;
    [[nodiscard]] bool has(const std::string& id, SimulationPhase phase) const;

    void run(SimulationPhase phase);

    [[nodiscard]] std::vector<std::string> list() const;
    [[nodiscard]] std::vector<std::string> list(SimulationPhase phase) const;

    [[nodiscard]] SystemPhaseBucket& bucket(SimulationPhase phase);
    [[nodiscard]] const SystemPhaseBucket& bucket(SimulationPhase phase) const;
    void sort(SimulationPhase phase);

    [[nodiscard]] u64 tick_count() const noexcept { return tick_count_; }
    void tick_next() noexcept { ++tick_count_; }

    [[nodiscard]] entt::registry& registry() noexcept { return reg_; }
    [[nodiscard]] const entt::registry& registry() const noexcept { return reg_; }

    static void auto_register(SimulationPhase p, std::string id, int prio, std::function<void(entt::registry&)> fn);
    static std::vector<AutoSystemEntry>& auto_registry();

    void sync_auto_systems();

private:
    static std::size_t idx(SimulationPhase p) noexcept {
        return static_cast<std::size_t>(p);
    }

    std::array<SystemPhaseBucket, kSimulationPhaseCount> buckets_{};
    std::size_t auto_consumed_ = 0;
    u64 tick_count_ = 0;
    entt::registry reg_;
};

struct SystemAutoReg {
    SystemAutoReg(SimulationPhase phase, std::string id, int priority, std::function<void(entt::registry&)> fn) {
        Simulation::auto_register(phase, std::move(id), priority, std::move(fn));
    }
};

}

#define FABRIKK_REGISTER_SYSTEM_IMPL(ID, PHASE, PRIORITY, FN, CNT)         \
    namespace {                                                            \
        const ::fabrikk::SystemAutoReg _fabrikk_sys_autoreg_##CNT{         \
            PHASE, ID, PRIORITY, (FN) };                                   \
    }

#define FABRIKK_REGISTER_SYSTEM(ID, PHASE, PRIORITY, FN)                   \
    FABRIKK_REGISTER_SYSTEM_IMPL(ID, PHASE, PRIORITY, FN, __COUNTER__)
