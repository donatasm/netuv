#include "NetUv.h"

namespace NetUv
{
    UvLoop::UvLoop()
    {
        _loop = uv_loop_new();
    }

    UvLoop::~UvLoop()
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
        return gcnew UvTcp(_loop);
    }
}