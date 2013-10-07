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

    IUvTcp^ UvLoop::InitUvTcp()
    {
        uv_tcp_t* tcp = new uv_tcp_t();
        return gcnew UvTcp(_loop, tcp);
    }

    IUvAsync^ UvLoop::InitUvAsync(UvAsyncCb^ asyncCb)
    {
        uv_async_t* async = new uv_async_t();
        return gcnew UvAsync(_loop, async, asyncCb);
    }
}