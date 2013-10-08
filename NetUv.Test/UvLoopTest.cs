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
        public void DefaultLoop()
        {
            Assert.IsNotNull(UvLoop.Default);
        }
    }
}
