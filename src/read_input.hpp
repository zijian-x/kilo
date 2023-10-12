#pragma once

#include "editor.hpp"
#include <functional>

str prompt_input(editor&, const str&,
        std::optional<std::function<void(editor&, const str&, int c)>> = {});
void process_key_press(editor&);
