#include "NetUv.h"

namespace NetUv
{
    UvTcp::UvTcp(uv_loop_t* loop)
    {
        _tcp = new uv_tcp_t();
        uv_tcp_init(loop, _tcp);
    }

    UvTcp::~UvTcp()
    {
        delete _tcp;
    }
}