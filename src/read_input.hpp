#pragma once

#include "editor_state.hpp"
#include <functional>

str prompt_input(editor_state& ed_state, const str& prompt,
        std::optional<std::function<void(editor_state&, const str&, int c)>> = {});
void process_key_press(editor_state&);
