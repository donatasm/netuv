#include "NetUv.h"

namespace NetUv
{
    void AsyncCb(uv_async_t* async, int status)
    {
        GCHandle gcThis = GCHandle::FromIntPtr(IntPtr(async->data));
        UvAsync^ target = (UvAsync^)gcThis.Target;

        if (status != 0)
        {
            target->_asyncCb(target, UvException::CreateFrom(async->loop));
        }
        else
        {
            target->_asyncCb(target, nullptr);
        }
    }

    UvAsync::UvAsync(uv_loop_t* loop, uv_async_t* async, UvAsyncCb^ asyncCb)
        : UvHandle((uv_handle_t*)async)
    {
        _async = async;
        _asyncCb = asyncCb;

        if (uv_async_init(loop, async, AsyncCb) != 0)
        {
            delete async;
            UvException::Throw(loop);
        }

        async->data = GetHandlePointer();
    }

    void UvAsync::Send()
    {
        if (uv_async_send(_async) != 0)
        {
            UvException::Throw(_async->loop);
        }
    }

    UvAsync::~UvAsync()
    {
        FreeHandle();
        this->!UvAsync();
    }

    UvAsync::!UvAsync()
    {
        if (_async != NULL)
        {
            delete _async;
            _async = NULL;
        }
    }
}