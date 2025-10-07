#pragma once

class TCPServer
{
public:
    virtual void start() = 0; // werver runs and can accept connections.
    ~TCPServer() = default;
};