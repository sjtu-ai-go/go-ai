//
// Created by lz on 12/4/16.
//

#include "simple_engine_base.hpp"
namespace engines {
    const char *SimpleEngineBase::KNOWN_COMMANDS[11] = {
            "genmove", "play", "protocol_version", "boardsize", "clear_board",
            "name", "version", "known_command", "quit", "time_settings", "time_left"
    };
}

