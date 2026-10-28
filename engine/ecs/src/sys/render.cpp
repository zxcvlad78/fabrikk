#include <entt/entity/registry.hpp>
#include <fabrikk/ecs/Simulation.hpp>
#include <fabrikk/ecs/SimulationPhase.hpp>

FABRIKK_REGISTER_SYSTEM("fabrikk:render", fabrikk::SimulationPhase::Render, 0,
[](entt::registry& reg) {

}
)
