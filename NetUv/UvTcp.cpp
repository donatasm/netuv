#include "NetUv.h"

namespace NetUv
{
    UvTcp::UvTcp(uv_tcp_t* tcp)
        : UvStream((uv_stream_t*)tcp)
    {
        _tcp = tcp;
    }

    UvTcp::~UvTcp()
    {
        FreeHandle();
        this->!UvTcp();
    }

    UvTcp::!UvTcp()
    {
        if (_tcp != NULL)
        {
            delete _tcp;
            _tcp = NULL;
        }
    }

    void UvTcp::Bind(String^ ip, int port)
    {
        struct sockaddr_in address = Ip4Address(ip, port);

        if (uv_tcp_bind(_tcp, address) != 0)
        {
            UvException::Throw(_tcp->loop);
        }
    }

    void ConnectCb(uv_connect_t* connect, int status)
    {
        uv_tcp_t* tcp = (uv_tcp_t*)connect->data;
        delete connect;

        GCHandle gcHandle = GCHandle::FromIntPtr(IntPtr(tcp->data));
        UvTcp^ target = (UvTcp^)gcHandle.Target;

        if (status != 0)
        {
            target->_connectCb(target, UvException::CreateFrom(tcp->loop));
        }
        else
        {
            target->_connectCb(target, nullptr);
        }
    }

    void UvTcp::Connect(String^ ip, int port, UvTcpCb^ connectCb)
    {
        struct sockaddr_in address = Ip4Address(ip, port);
        uv_connect_t* connect = new uv_connect_t();
        connect->data = _tcp;

        if (uv_tcp_connect(connect, _tcp, address, ConnectCb) != 0)
        {
            delete connect;
            UvException::Throw(_tcp->loop);
        }

        _connectCb = connectCb;

        GCHandle gcHandle = GCHandle::Alloc(this);
        _tcp->data = GCHandle::ToIntPtr(gcHandle).ToPointer();
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