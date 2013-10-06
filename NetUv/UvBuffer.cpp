#include "NetUv.h"

namespace NetUv
{
    UvBuffer::UvBuffer(array<byte>^ buffer, int offset, int count)
    {
        if (buffer == nullptr)
            throw gcnew ArgumentNullException("buffer");

        if (buffer->Length == 0)
            throw gcnew ArgumentException("Buffer length should be greater than 0.",
                "buffer");

        if (offset < 0)
            throw gcnew ArgumentOutOfRangeException("offset",
                "Offset should be greater than or equal to 0.");
        if (offset >= buffer->Length)
            throw gcnew ArgumentOutOfRangeException("offset",
                "Offset should be less than buffer length.");

        if (count < 1)
            throw gcnew ArgumentOutOfRangeException("count",
                "Count should be greater than 0.");
        if (count > buffer->Length)
            throw gcnew ArgumentOutOfRangeException("count",
                "Count should be less than or equal to buffer length.");

        if (offset + count > buffer->Length)
            throw gcnew ArgumentOutOfRangeException(
                "Offset + count should be less than or equal to buffer length.");

        _buffer = buffer;
        _offset = offset;
        _count = count;
    }
}