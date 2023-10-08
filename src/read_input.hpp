#pragma once

#include <optional>

#include "editor_state.hpp"

str prompt_input(editor_state& ed_state, const str& prompt);
void process_key_press(editor_state&);
