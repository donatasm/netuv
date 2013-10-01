#include "NetUv.h"

namespace NetUv
{
    UvHandle::UvHandle(uv_handle_t* handle)
    {
        _handle = handle;
        _gcThis = GCHandle::Alloc(this);
    }

    void CloseCb(uv_handle_t* handle)
    {
        GCHandle gcThis = GCHandle::FromIntPtr(IntPtr(handle->data));

        UvHandle^ target = (UvHandle^)gcThis.Target;
        target->_closeCb((IDisposable^)target);
    }

    void UvHandle::Close(UvCloseCb^ closeCb)
    {
        uv_close(_handle, CloseCb);

        _closeCb = closeCb;

        _handle->data = GetHandlePointer();
    }

    void* UvHandle::GetHandlePointer()
    {
        return GCHandle::ToIntPtr(_gcThis).ToPointer();
    }

    void UvHandle::FreeHandle()
    {
        _gcThis.Free();
    }
}