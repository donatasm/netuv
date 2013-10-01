using NUnit.Framework;

namespace NetUv.Test
{
    [TestFixture]
    internal sealed class UvLoopTest
    {
        [Test]
        public void EmptyLoop()
        {
            using (new UvLoop())
            {
            }
        }

        [Test]
        public void Run()
        {
            using (var loop = new UvLoop())
            {
                loop.Run();
            }
        }

        [Test]
        public void InitUvTcp()
        {
            using (var loop = new UvLoop())
            using (loop.InitUvTcp())
            {
            }
        }

        [Test]
        public void Dispose()
        {
            using (var loop = new UvLoop())
            {
                loop.Dispose();
                loop.Dispose();
            }
        }
    }
}
