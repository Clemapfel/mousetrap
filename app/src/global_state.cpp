#include <app/global_state.hpp>

namespace mousetrap
{
    ProjectState& get_active_state()
    {
        return *state::project_states.at(state::active_project);
    }
}