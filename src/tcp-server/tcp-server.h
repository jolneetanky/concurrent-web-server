#pragma once

class TCPServer
{
public:
    void startServer(); // werver runs and can accept connections.
    ~TCPServer() = default;
};