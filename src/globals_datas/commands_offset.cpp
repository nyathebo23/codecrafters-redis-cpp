#include "commands_offset.h"

unsigned int CommandsOffset::get_cmd_offset() {
    return this->commands_offset;
}

unsigned int CommandsOffset::get_prec_cmd_offset() {
    return this->prec_commands_offset;
}

CommandsOffset::CommandsOffset(){
    this->commands_offset = 0;
    this->prec_commands_offset = 0;
}

void CommandsOffset::set(unsigned short offset){
    CommandsOffset::prec_commands_offset = CommandsOffset::commands_offset;
    CommandsOffset::commands_offset += offset;
};

