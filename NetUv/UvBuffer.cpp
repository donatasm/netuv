#include "NetUv.h"

namespace NetUv
{
    UvBuffer::UvBuffer(array<byte>^ buffer, int offset, int count)
    {
        _buffer = buffer;
        _offset = offset;
        _count = count;
    }
}