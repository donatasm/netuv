#include "NetUv.h"

namespace NetUv
{
    UvException::UvException(String^ message, String^ error)
        : Exception(message)
    {
        _error = error;
    }

    UvException^ UvException::CreateFrom(uv_loop_t* loop)
    {
        uv_err_t error = uv_last_error(loop);
        String^ message = gcnew String(uv_strerror(error));
        String^ errorName = gcnew String(uv_err_name(error));
        return gcnew UvException(message, errorName);
    }

    void UvException::Throw(uv_loop_t* loop)
    {
        throw CreateFrom(loop);
    }

    String^ UvException::Error::get()
    {
        return _error;
    }
}