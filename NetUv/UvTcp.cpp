#include "NetUv.h"

namespace NetUv
{
    UvTcp::UvTcp(uv_loop_t* loop)
    {
        _loop = loop;
        _tcp = new uv_tcp_t();
        uv_tcp_init(loop, _tcp);
    }

    UvTcp::~UvTcp()
    {
        if (_tcp != NULL)
        {
            delete _tcp;
            _tcp = NULL;
        }
    }

    void ConnectCb(uv_connect_t* connect, int status)
    {
        uv_tcp_t* tcp = (uv_tcp_t*)connect->data;
        delete connect;

        GCHandle handle = GCHandle::FromIntPtr(IntPtr(tcp->data));

        try
        {
            UvTcp^ target = (UvTcp^)handle.Target;

            if (status != 0)
            {
                target->_callback(target, UvException::CreateFrom(tcp->loop));
            }
            else
            {
                target->_callback(target, nullptr);
            }
        }
        finally
        {
            handle.Free();
        }
    }

    void UvTcp::Connect(String^ ip, int port, UvTcpCb^ callback)
    {
        struct sockaddr_in address = Ip4Address(ip, port);
        uv_connect_t* connect = new uv_connect_t();
        connect->data = _tcp;

        if (uv_tcp_connect(connect, _tcp, address, ConnectCb) != 0)
        {
            delete connect;
            callback(this, UvException::CreateFrom(_loop));
            return;
        }

        _callback = callback;

        GCHandle handle = GCHandle::Alloc(this);
        _tcp->data = GCHandle::ToIntPtr(handle).ToPointer();
    }

    void ListenCb(uv_stream_t* server, int status)
    {
        uv_tcp_t* tcp = (uv_tcp_t*)server;

        GCHandle handle = GCHandle::FromIntPtr(IntPtr(tcp->data));

        try
        {
            UvTcp^ target = (UvTcp^)handle.Target;

            if (status != 0)
            {
                target->_callback(target, UvException::CreateFrom(tcp->loop));
            }
            else
            {
                target->_callback(target, nullptr);
            }
        }
        finally
        {
            handle.Free();
        }
    }

    void UvTcp::Listen(String^ ip, int port, int backlog, UvTcpCb^ callback)
    {
        struct sockaddr_in address = Ip4Address(ip, port);

        if (uv_tcp_bind(_tcp, address) != 0)
        {
            callback(this, UvException::CreateFrom(_loop));
            return;
        }

        if (uv_listen((uv_stream_t*)_tcp, backlog, ListenCb) != 0)
        {
            callback(this, UvException::CreateFrom(_loop));
            return;
        }

        _callback = callback;

        GCHandle handle = GCHandle::Alloc(this);
        _tcp->data = GCHandle::ToIntPtr(handle).ToPointer();
    }

    void UvTcp::Close()
    {
        uv_close((uv_handle_t*)_tcp, NULL);
    }

    struct sockaddr_in UvTcp::Ip4Address(String^ ip, int port)
    {
        struct sockaddr_in address;

        memset(&address, 0, sizeof(struct sockaddr_in));

        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = (unsigned long)IPAddress::Parse(ip)->Address;

        return address;
    }
}