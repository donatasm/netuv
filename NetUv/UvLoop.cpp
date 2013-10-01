#include "NetUv.h"

namespace NetUv
{
    UvLoop::UvLoop()
    {
        _loop = uv_loop_new();
    }

    UvLoop::~UvLoop()
    {
        this->!UvLoop();
    }

    UvLoop::!UvLoop()
    {
        if (_loop != NULL)
        {
            uv_loop_delete(_loop);
            _loop = NULL;
        }
    }

    void UvLoop::Run()
    {
        uv_run(_loop, UV_RUN_DEFAULT);
    }

    UvTcp^ UvLoop::InitUvTcp()
    {
        uv_tcp_t* tcp = new uv_tcp_t();

        if (uv_tcp_init(_loop, tcp) != 0)
        {
            delete tcp;
            UvException::Throw(_loop);
        }

        return gcnew UvTcp(tcp);
    }
}