#include <fabrikk/ecs/Simulation.hpp>
#include <fabrikk/ecs/ctx/SimulationContext.hpp>

#include <algorithm>
#include <iostream>

namespace fabrikk {

std::vector<Simulation::AutoSystemEntry>& Simulation::auto_registry() {
    static std::vector<AutoSystemEntry> reg;
    return reg;
}

void Simulation::auto_register(SimulationPhase p, std::string id, int prio, std::function<void(entt::registry&)> fn) {
    if (id.empty()) {
        std::cerr << "[Simulation] auto_register: empty id\n";
        return;
    }
    if (!valid(p)) {
        std::cerr << "[Simulation] auto_register: invalid phase\n";
        return;
    }
    auto_registry().push_back({p, SystemEntry{std::move(id), std::move(fn), prio}});
}

Simulation::Simulation() {
    reg_.ctx().emplace<ctx::SimulationContext>();
    sync_auto_systems();
}

void Simulation::sync_auto_systems() {
    auto& src = auto_registry();
    for (; auto_consumed_ < src.size(); ++auto_consumed_) {
        const auto& a = src[auto_consumed_];
        if (!valid(a.phase)) continue;

        auto& b = buckets_[idx(a.phase)];
        b.entries.push_back(a.entry);
        b.sorted = false;
    }
}

Simulation::SystemPhaseBucket& Simulation::bucket(SimulationPhase phase) {
    return buckets_[idx(phase)];
}

const Simulation::SystemPhaseBucket&
Simulation::bucket(SimulationPhase phase) const {
    return buckets_[idx(phase)];
}

void Simulation::emplace(SimulationPhase phase, SystemEntry entry) {
    if (entry.id.empty()) {
        std::cerr << "[Simulation] emplace: empty id\n";
        return;
    }
    if (!valid(phase)) {
        std::cerr << "[Simulation] emplace: invalid phase\n";
        return;
    }

    auto& b = buckets_[idx(phase)];
    for (auto& e : b.entries) {
        if (e.id == entry.id) {
            e.priority = entry.priority;
            e.func = std::move(entry.func);
            b.sorted = false;
            return;
        }
    }
    b.entries.push_back(std::move(entry));
    b.sorted = false;
}

void Simulation::clear() {
    for (auto& b : buckets_) {
        b.entries.clear();
        b.sorted = true;
    }
}

void Simulation::clear(SimulationPhase phase) {
    if (!valid(phase)) {
        std::cerr << "[Simulation] clear: invalid phase\n";
        return;
    }
    auto& b = buckets_[idx(phase)];
    b.entries.clear();
    b.sorted = true;
}

void Simulation::remove(const std::string& id) {
    if (id.empty()) {
        std::cerr << "[Simulation] remove: empty id\n";
        return;
    }
    for (auto& b : buckets_) {
        b.entries.erase(
            std::remove_if(
                b.entries.begin(), b.entries.end(),[&](const SystemEntry& e) { return e.id == id; }
            ),
            b.entries.end());
    }
}

void Simulation::remove(const std::string& id, SimulationPhase phase) {
    if (id.empty()) {
        std::cerr << "[Simulation] remove: empty id\n";
        return;
    }
    if (!valid(phase)) {
        std::cerr << "[Simulation] remove: invalid phase\n";
        return;
    }
    auto& v = buckets_[idx(phase)].entries;
    v.erase(std::remove_if(
        v.begin(), v.end(), [&](const SystemEntry& e) { return e.id == id; }), v.end()
    );
}

bool Simulation::has(const std::string& id) const {
    if (id.empty()) return false;
    for (const auto& b : buckets_) {
        for (const auto& e : b.entries) if (e.id == id) return true;
    }
    return false;
}

bool Simulation::has(const std::string& id, SimulationPhase phase) const {
    if (id.empty())   return false;
    if (!valid(phase)) return false;
    for (const auto& e : buckets_[idx(phase)].entries)
        if (e.id == id) return true;
    return false;
}

std::vector<std::string> Simulation::list() const {
    std::vector<std::string> out;
    for (const auto& b : buckets_)
        for (const auto& e : b.entries) out.push_back(e.id);
    return out;
}

std::vector<std::string> Simulation::list(SimulationPhase phase) const {
    std::vector<std::string> out;
    if (!valid(phase)) {
        std::cerr << "[Simulation] list: invalid phase\n";
        return out;
    }
    const auto& b = buckets_[idx(phase)];
    out.reserve(b.entries.size());
    for (const auto& e : b.entries) out.push_back(e.id);
    return out;
}

void Simulation::sort(SimulationPhase phase) {
    if (!valid(phase)) {
        std::cerr << "[Simulation] sort: invalid phase\n";
        return;
    }
    auto& b = buckets_[idx(phase)];
    if (b.sorted) return;
    std::stable_sort(b.entries.begin(), b.entries.end(),
                     [](const SystemEntry& a, const SystemEntry& b) {
                         return a.priority < b.priority;
                     }
                    );
    b.sorted = true;
}

void Simulation::run(SimulationPhase phase) {
    if (!valid(phase)) {
        std::cerr << "[Simulation] run: invalid phase\n";
        return;
    }
    sync_auto_systems();
    sort(phase);

    auto& entries = buckets_[idx(phase)].entries;
    const auto n = entries.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (entries[i].func) entries[i].func(reg_);
    }
}

}