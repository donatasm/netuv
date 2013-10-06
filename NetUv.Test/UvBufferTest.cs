using System;
using NUnit.Framework;

namespace NetUv.Test
{
    [TestFixture]
    internal sealed class UvBufferTest
    {
        [Test]
        public void NullBufferThrowsArgumentNullException()
        {
            var exception = Assert.Throws<ArgumentNullException>(
                () => new UvBuffer(null, 0, 1));
            Assert.AreEqual("buffer", exception.ParamName);
        }

        [Test]
        public void BufferLengthIs0ThrowsArgumentNullException()
        {
            var exception = Assert.Throws<ArgumentException>(
                () => new UvBuffer(new byte[0], 0, 1));
            Assert.AreEqual("buffer", exception.ParamName);
        }

        [TestCase(2, Description = "Offset is greater than length")]
        [TestCase(1, Description = "Offset is equal to length")]
        [TestCase(-1, Description = "Offset is less than 0")]
        public void OffsetThrowsArgumentOutOfRangeException(int offset)
        {
            var buffer = new byte[1];
            var exception = Assert.Throws<ArgumentOutOfRangeException>(
                () => new UvBuffer(buffer, offset, buffer.Length));
            Assert.AreEqual("offset", exception.ParamName);
        }

        [TestCase(2, Description = "Count is greater than buffer length")]
        [TestCase(0, Description = "Count equals 0")]
        [TestCase(-1, Description = "Count is less than 0")]
        public void CountThrowsArgumentOutOfRangeException(int count)
        {
            var buffer = new byte[1];
            var exception = Assert.Throws<ArgumentOutOfRangeException>(
                () => new UvBuffer(buffer, 0, count));
            Assert.AreEqual("count", exception.ParamName);
        }

        [TestCase(0, 4)]
        [TestCase(1, 3)]
        [TestCase(2, 2)]
        public void OffsetCountThrowsArgumentOutOfRangeException(int offset, int count)
        {
            var buffer = new byte[3];
            Assert.Throws<ArgumentOutOfRangeException>(
                () => new UvBuffer(buffer, offset, count));
        }
    }
}
