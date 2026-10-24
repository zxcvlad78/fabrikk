#include <fabrikk/ecs/Simulation.hpp>
#include <iostream>

namespace fabrikk {
Simulation::SystemPhaseBucket& Simulation::bucket(SimulationPhase phase) {
    return _buckets[static_cast<std::size_t>(phase)];
}

void Simulation::emplace(SimulationPhase phase, std::function<void()> func, const std::string& id, int priority) {
    if (id.empty()) {
        std::cerr << "[Simulation] emplace: empty id\n";
        return;
    }
    if (!valid(phase)) {
        std::cerr << "[Simulation] emplace: invalid phase\n";
        return;
    }

    auto& bucket = _buckets[static_cast<std::size_t>(phase)];

    for (auto& e : bucket.entries) {
        if (e.id == id) {
            e.priority = priority;
            e.func = std::move(func);
            bucket.sorted = false;
            return;
        }
    }

    bucket.entries.push_back({id, priority, std::move(func)});
    bucket.sorted = false;
}

void Simulation::remove(const std::string& id) {
    if (id.empty()) {
        std::cerr << "[Simulation] remove: empty id\n";
        return;
    }
    for (auto& b : _buckets) {
        b.entries.erase(
            std::remove_if(b.entries.begin(), b.entries.end(),
                [&](const SystemEntry& e) { return e.id == id; }),
            b.entries.end()
        );
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

    auto& b = bucket(phase);
    b.entries.erase(
        std::remove_if(
            b.entries.begin(),
            b.entries.end(),
            [&](const SystemEntry& e) { return e.id == id; }
        ),
        b.entries.end()
    );
}

bool Simulation::has(const std::string& id) {
    if (id.empty()) return false;

    for (auto& b : _buckets) {
        for (auto& e : b.entries) if (e.id == id) return true;
    }
    return false;
}

bool Simulation::has(const std::string& id, SimulationPhase phase) {
    if (!valid(phase)) return false;

    for (auto& e : bucket(phase).entries)
        if (e.id == id) return true;
    
    return false;
}

std::vector<std::string> Simulation::list() {
    std::vector<std::string> out;
    for (auto& b : _buckets) {
        for (auto& e : b.entries) out.push_back(e.id);
    }
    return out;
}

std::vector<std::string> Simulation::list(SimulationPhase phase) {
    auto& b = bucket(phase);
    std::vector<std::string> out;

    if (!valid(phase)) {
        std::cerr << "[Simulation] list: invalid phase\n";
        return out;
    }

    out.reserve(b.entries_count);

    for (auto& e : b.entries)
        out.push_back(e.id);
    
    return out;
}

void Simulation::sort(SimulationPhase phase) {
    auto& b = bucket(phase);
    if (b.sorted) return;

    std::stable_sort(
        b.entries.begin(),
        b.entries.end(),
        [](const SystemEntry& a, const SystemEntry& b) { return a.priority < b.priority; }
    );

    b.sorted = true;
}

void Simulation::clear() {
    for (auto& b : _buckets) {
        b.entries.clear();
        b.sorted = true;
    }
}

void Simulation::clear(SimulationPhase phase) {
    if (!valid(phase)) {
        std::cerr << "[Simulation] clear: invalid phase\n";
        return;
    }

    auto& b = bucket(phase);
    b.entries.clear();
    b.sorted = true;
}

void Simulation::run(SimulationPhase phase) {
    if (!valid(phase)) {
        std::cerr << "[Simulation] run: invalid phase\n";
        return;
    }

    sort(phase);
    auto& entries = bucket(phase).entries;
    for (size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].func) entries[i].func();
    }
}

u64 Simulation::tick_count() const { return tick_count_; }
void Simulation::tick_next() { tick_count_++; }

entt::registry& Simulation::registry() { return reg_; }

}