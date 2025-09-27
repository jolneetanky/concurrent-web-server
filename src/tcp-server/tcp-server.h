#pragma once

class TCPServer
{
public:
    virtual void startServer() = 0; // werver runs and can accept connections.
    ~TCPServer() = default;
};