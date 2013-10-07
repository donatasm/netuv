#include "uv.h"

using namespace System;
using namespace System::Net;
using namespace System::Runtime::InteropServices;

namespace NetUv
{
    public delegate void UvCloseCb(IDisposable^ handle);

    public interface class IUvHandle
    {
        void Close(UvCloseCb^ closeCb);
    };

    public ref class UvHandle abstract : IUvHandle
    {
    public:
        virtual void Close(UvCloseCb^ closeCb);
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

    public interface class IUvStream : IUvHandle
    {
        void Accept(UvStream^ stream);
        void Listen(int backlog, UvStreamCb^ listenCb);
        void Write(UvBuffer^ buffer, UvStreamCb^ writeCb);
        void ReadStart(UvAllocCb^ allocCb, UvStreamReadCb^ readCb);
        void ReadStop();
        void Shutdown(UvStreamCb^ shutdownCb);
    };

    public ref class UvStream abstract : UvHandle, IUvStream
    {
    public:
        static initonly int EOF = -1;
        virtual void Accept(UvStream^ stream);
        virtual void Listen(int backlog, UvStreamCb^ listenCb);
        virtual void Write(UvBuffer^ buffer, UvStreamCb^ writeCb);
        virtual void ReadStart(UvAllocCb^ allocCb, UvStreamReadCb^ readCb);
        virtual void ReadStop();
        virtual void Shutdown(UvStreamCb^ shutdownCb);
    internal:
        UvStream(uv_stream_t* stream);
        UvStreamCb^ _listenCb;
        UvStreamCb^ _writeCb;
        UvStreamReadCb^ _readCb;
        UvAllocCb^ _allocCb;
        UvStreamCb^ _shutdownCb;
        GCHandle _gcWriteBuf;
        GCHandle _gcReadBuf;
    private:
        initonly uv_stream_t* _stream;
    };

    ref class UvTcp;
    public delegate void UvTcpCb(UvTcp^ tcp, Exception^ exception);

    public interface class IUvTcp : IUvStream
    {
        void Bind(String^ ip, int port);
        void Connect(String^ ip, int port, UvTcpCb^ connectCb);
    };

    public ref class UvTcp sealed : UvStream, IUvTcp
    {
    public:
        virtual void Bind(String^ ip, int port);
        virtual void Connect(String^ ip, int port, UvTcpCb^ connectCb);
        ~UvTcp();
    internal:
        UvTcp(uv_loop_t*, uv_tcp_t* tcp);
        UvTcpCb^ _connectCb;
    private:
        uv_tcp_t* _tcp;
        static struct sockaddr_in Ip4Address(String^ ip, int port);
        !UvTcp();
    };

    public interface class IUvAsync : IUvHandle, IDisposable
    {
        void Send();
    };

    ref class UvAsync;
    public delegate void UvAsyncCb(UvAsync^ async, Exception^ exception);

    public ref class UvAsync : UvHandle, IUvAsync
    {
    public:
        virtual void Send();
        ~UvAsync();
    internal:
        UvAsync(uv_loop_t* loop, uv_async_t* async, UvAsyncCb^ asyncCb);
        UvAsyncCb^ _asyncCb;
    private:
        uv_async_t* _async;
        !UvAsync();
    };

    public interface class IUvLoop
    {
        void Run();
        IUvTcp^ InitUvTcp();
        IUvAsync^ InitUvAsync(UvAsyncCb^ asyncCb);
    };

    public ref class UvLoop sealed : IUvLoop
    {
    public:
        UvLoop();
        ~UvLoop();
        !UvLoop();
        virtual void Run();
        virtual IUvTcp^ InitUvTcp();
        virtual IUvAsync^ InitUvAsync(UvAsyncCb^ asyncCb);
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