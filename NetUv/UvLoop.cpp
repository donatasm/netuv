#include "NetUv.h"

namespace NetUv
{
    UvLoop::UvLoop()
    {
        _loop = uv_loop_new();
    }

    UvLoop::~UvLoop()
    {
        uv_loop_delete(_loop);
    }

    void UvLoop::Run()
    {
        uv_run(_loop, UV_RUN_DEFAULT);
    }

    UvTcp^ UvLoop::InitUvTcp()
    {
        return gcnew UvTcp(_loop);
    }
}