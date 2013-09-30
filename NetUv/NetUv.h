#include "uv.h"

using namespace System;
using namespace System::Net;
using namespace System::Runtime::InteropServices;

namespace NetUv
{
    ref class UvTcp;
    public delegate void UvTcpCb(UvTcp^ tcp, Exception^ exception);

    public ref class UvTcp sealed
    {
    public:
        ~UvTcp();
        void Connect(String^ ip, int port, UvTcpCb^ callback);
        void Listen(String^ ip, int port, int backlog, UvTcpCb^ callback);
        void Close();
    internal:
        UvTcp(uv_loop_t* loop);
        UvTcpCb^ _callback;
    private:
        uv_tcp_t* _tcp;
        uv_loop_t* _loop;
        static struct sockaddr_in Ip4Address(String^ ip, int port);
    };

    public ref class UvLoop sealed
    {
    public:
        UvLoop();
        ~UvLoop();
        void Run();
        UvTcp^ InitUvTcp();
    private:
        uv_loop_t* _loop;
    };

    public ref class UvException sealed : Exception
    {
    public:
        property String^ ErrorName { String^ get(); }
    internal:
        UvException(String^ message, String^ errorName);
        static UvException^ CreateFrom(uv_loop_t* loop);
        static void Throw(uv_loop_t* loop);
    private:
        initonly String^ _errorName;
    };
}