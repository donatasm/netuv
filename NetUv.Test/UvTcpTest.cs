using NUnit.Framework;

namespace NetUv.Test
{
    [TestFixture]
    internal sealed class UvTcpTest
    {
        [Test]
        public void ConnectThrowsUvException()
        {
            using (var loop = new UvLoop())
            using (var client = loop.InitUvTcp())
            {
                UvException uvException = null;
                UvTcp uvTcp = null;

                client.Connect("127.0.0.1", 1337,
                    (tcp, exception) =>
                        {
                            uvTcp = tcp;
                            uvException = (UvException)exception;
                        });

                loop.Run();

                Assert.IsNotNull(uvTcp);
                Assert.IsNotNull(uvException);
                Assert.AreEqual("ECONNREFUSED", uvException.ErrorName);
            }
        }
    }
}