#include "uv.h"

using namespace System;
using namespace System::Net;
using namespace System::Runtime::InteropServices;

namespace NetUv
{
    ref class UvHandle;
    public delegate void UvCloseCb(IDisposable^ handle);

    public ref class UvHandle abstract
    {
    public:
        void Close(UvCloseCb^ closeCb);
    internal:
        UvHandle(uv_handle_t* handle);
        void* GetHandlePointer();
        void FreeHandle();
        UvCloseCb^ _closeCb;
    private:
        initonly uv_handle_t* _handle;
        GCHandle _gcThis;
    };

    ref struct UvBuffer;
    public delegate UvBuffer^ UvAllocCb(int suggestedBufferSize);

    public ref struct UvBuffer sealed
    {
    public:
        UvBuffer(array<byte>^ buffer, int offset, int count);
    internal:
        initonly array<byte>^ _buffer;
        initonly int _offset;
        initonly int _count;
    };

    ref class UvStream;
    public delegate void UvStreamCb(UvStream^ stream, Exception^ exception);
    public delegate void UvStreamReadCb(UvStream^ stream, int read, UvBuffer^ buffer);

    public ref class UvStream abstract : UvHandle
    {
    public:
        static initonly int EOF = -1;
        void Accept(UvStream^ client);
        void Listen(int backlog, UvStreamCb^ listenCb);
        void Write(UvBuffer^ buffer, UvStreamCb^ writeCb);
        void ReadStart(UvAllocCb^ allocCb, UvStreamReadCb^ readCb);
    internal:
        UvStream(uv_stream_t* stream);
        UvStreamCb^ _listenCb;
        UvStreamCb^ _writeCb;
        UvStreamReadCb^ _readCb;
        UvAllocCb^ _allocCb;
        GCHandle _gcWriteBuf;
        GCHandle _gcReadBuf;
    private:
        initonly uv_stream_t* _stream;
    };

    ref class UvTcp;
    public delegate void UvTcpCb(UvTcp^ tcp, Exception^ exception);

    public ref class UvTcp sealed : UvStream
    {
    public:
        void Bind(String^ ip, int port);
        void Connect(String^ ip, int port, UvTcpCb^ connectCb);
        ~UvTcp();
    internal:
        UvTcp(uv_tcp_t* tcp);
        UvTcpCb^ _connectCb;
    private:
        uv_tcp_t* _tcp;
        static struct sockaddr_in Ip4Address(String^ ip, int port);
        !UvTcp();
    };

    public ref class UvLoop sealed
    {
    public:
        UvLoop();
        ~UvLoop();
        !UvLoop();
        void Run();
        UvTcp^ InitUvTcp();
    private:
        uv_loop_t* _loop;
    };

    public ref class UvException sealed : Exception
    {
    public:
        property String^ Error { String^ get(); }
    internal:
        UvException(String^ message, String^ errorName);
        static UvException^ CreateFrom(uv_loop_t* loop);
        static void Throw(uv_loop_t* loop);
    private:
        initonly String^ _error;
    };
}