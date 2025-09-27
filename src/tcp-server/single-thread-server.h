#pragma once
#include "tcp-server.h"

class SingleThreadServer : public TCPServer
{
public:
    void startServer() override;
};