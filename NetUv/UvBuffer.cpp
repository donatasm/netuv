#include "NetUv.h"

namespace NetUv
{
    UvBuffer::UvBuffer(array<byte>^ buffer, int offset, int count)
    {
        if (buffer == nullptr)
            throw gcnew ArgumentNullException("buffer");

        if (buffer->Length == 0)
            throw gcnew ArgumentException("Buffer length should be greater than 0. "
                + ParamsMessage(buffer, offset, count), "buffer");

        if (offset < 0)
            throw gcnew ArgumentOutOfRangeException("offset",
                "Offset should be greater than or equal to 0. "
                + ParamsMessage(buffer, offset, count));
        if (offset >= buffer->Length)
            throw gcnew ArgumentOutOfRangeException("offset",
                "Offset should be less than buffer length. "
                + ParamsMessage(buffer, offset, count));

        if (count < 1)
            throw gcnew ArgumentOutOfRangeException("count",
                "Count should be greater than 0. "
                + ParamsMessage(buffer, offset, count));
        if (count > buffer->Length)
            throw gcnew ArgumentOutOfRangeException("count",
                "Count should be less than or equal to buffer length. "
                + ParamsMessage(buffer, offset, count));

        if (offset + count > buffer->Length)
            throw gcnew ArgumentOutOfRangeException(
                "Offset + count should be less than or equal to buffer length. "
                + ParamsMessage(buffer, offset, count));

        _buffer = buffer;
        _offset = offset;
        _count = count;
    }

    int UvBuffer::Offset::get()
    {
        return _offset;
    }

    int UvBuffer::Count::get()
    {
        return _count;
    }

    array<byte>^ UvBuffer::Array::get()
    {
        return _buffer;
    }

    String^ UvBuffer::ParamsMessage(array<byte>^ buffer, int offset, int count)
    {
        return String::Format("Buffer.Length={0}, offset={1}, count={2}.",
            buffer->Length, offset, count);
    }
}