#ifndef TRANSACTIONS_CMDS_PROCESSING_H
#define TRANSACTIONS_CMDS_PROCESSING_H

#include <string>
#include <map>
#include <vector>
#include <any>


class TransactionsCmdsProcessing {

    public:

        static void multi(int dest_fd);
        static void exec(bool& is_queue_active, std::vector<std::string>& cmds_to_exec, int dest_fd);
        static void discard(bool& is_queue_active, std::vector<std::string>& cmds_to_exec, int dest_fd);

};


#endif