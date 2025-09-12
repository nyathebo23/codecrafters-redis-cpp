#ifndef COMMANDS_OFFSET_DATAS_H
#define COMMANDS_OFFSET_DATAS_H

class CommandsOffset {
    public:
        void set(unsigned short offset);
        CommandsOffset();
        unsigned int get_prec_cmd_offset();
        unsigned int get_cmd_offset();

    private:
        unsigned int prec_commands_offset;
        unsigned int commands_offset;
};

#endif