#include "NetUv.h"

namespace NetUv
{
    UvStream::UvStream(uv_stream_t* stream)
        : UvHandle((uv_handle_t*)stream)
    {
        _stream = stream;
    }

    void UvStream::Accept(UvStream^ client)
    {
        if (uv_accept(_stream, client->_stream) != 0)
        {
            UvException::Throw(_stream->loop);
        }
    }

    void ListenCb(uv_stream_t* server, int status)
    {
        GCHandle gcThis = GCHandle::FromIntPtr(IntPtr(server->data));
        UvTcp^ target = (UvTcp^)gcThis.Target;

        if (status != 0)
        {
            target->_listenCb(target, UvException::CreateFrom(server->loop));
        }
        else
        {
            target->_listenCb(target, nullptr);
        }
    }

    void UvStream::Listen(int backlog, UvStreamCb^ listenCb)
    {
        if (uv_listen(_stream, backlog, ListenCb) != 0)
        {
            UvException::Throw(_stream->loop);
        }

        _listenCb = listenCb;

        _stream->data = GetHandlePointer();
    }

    void WriteCb(uv_write_t* write, int status)
    {
        uv_stream_t* stream = (uv_stream_t*)write->data;
        delete write;

        GCHandle gcThis = GCHandle::FromIntPtr(IntPtr(stream->data));
        UvStream^ target = (UvStream^)gcThis.Target;

        try
        {
            if (status != 0)
            {
                target->_writeCb(target, UvException::CreateFrom(stream->loop));
            }
            else
            {
                target->_writeCb(target, nullptr);
            }
        }
        finally
        {
            GCHandle gcWriteBuf = target->_gcWriteBuf;

            if (gcWriteBuf.IsAllocated)
            {
                gcWriteBuf.Free();
            }
        }
    }

    void UvStream::Write(UvBuffer^ buffer, UvStreamCb^ writeCb)
    {
        uv_write_t* write = new uv_write_t();
        write->data = _stream;

        _gcWriteBuf = GCHandle::Alloc(buffer->_buffer, GCHandleType::Pinned);

        char* base = (char*)Marshal::UnsafeAddrOfPinnedArrayElement(
            buffer->_buffer, buffer->_offset).ToPointer();
        uv_buf_t buf = uv_buf_init(base, buffer->_count);

        if (uv_write(write, _stream, &buf, 1, WriteCb) != 0)
        {
            delete write;
            _gcWriteBuf.Free();
            UvException::Throw(_stream->loop);
        }

        _writeCb = writeCb;

        _stream->data = GetHandlePointer();
    }

    uv_buf_t AllocCb(uv_handle_t* handle, size_t size)
    {
        GCHandle gcThis = GCHandle::FromIntPtr(IntPtr(handle->data));
        UvStream^ target = (UvStream^)gcThis.Target;

        UvBuffer^ buffer = target->_allocCb((int)size);
        target->_gcReadBuf = GCHandle::Alloc(buffer->_buffer, GCHandleType::Pinned);

        char* base = (char*)Marshal::UnsafeAddrOfPinnedArrayElement(
            buffer->_buffer, buffer->_offset).ToPointer();

        return uv_buf_init(base, buffer->_count);
    }

    void ReadCb(uv_stream_t* stream, ssize_t nread, uv_buf_t buf)
    {
        GCHandle gcThis = GCHandle::FromIntPtr(IntPtr(stream->data));
        UvStream^ target = (UvStream^)gcThis.Target;

        GCHandle gcReadBuf = target->_gcReadBuf;

        try
        {
            if (nread > 0)
            {
                array<byte>^ buffer = (array<byte>^)gcReadBuf.Target;
                target->_readCb(target, (int)nread, gcnew UvBuffer(buffer, 0, nread));
            }
            else
            {
                target->_readCb(target, (int)nread, nullptr);
            }
        }
        finally
        {
            if (gcReadBuf.IsAllocated)
            {
                gcReadBuf.Free();
            }
        }
    }

    void UvStream::ReadStart(UvAllocCb^ allocCb, UvStreamReadCb^ readCb)
    {
        if (uv_read_start(_stream, AllocCb, ReadCb) != 0)
        {
            UvException::Throw(_stream->loop);
        }

        _allocCb = allocCb;
        _readCb = readCb;

        _stream->data = GetHandlePointer();
    }

    void ShutdownCb(uv_shutdown_t* shutdown, int status)
    {
        uv_stream_t* stream = (uv_stream_t*)shutdown->data;
        delete shutdown;

        GCHandle gcThis = GCHandle::FromIntPtr(IntPtr(stream->data));
        UvStream^ target = (UvStream^)gcThis.Target;

        if (status != 0)
        {
            target->_shutdownCb(target, UvException::CreateFrom(stream->loop));
        }
        else
        {
            target->_shutdownCb(target, nullptr);
        }
    }

    void UvStream::Shutdown(UvStreamCb^ shutdownCb)
    {
        uv_shutdown_t* shutdown = new uv_shutdown_t();
        shutdown->data = _stream;

        if (uv_shutdown(shutdown, _stream, ShutdownCb) != 0)
        {
            delete shutdown;
            UvException::Throw(_stream->loop);
        }

        _shutdownCb = shutdownCb;

        _stream->data = GetHandlePointer();
    }
}