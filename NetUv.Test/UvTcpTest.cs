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

        [Test]
        public void ListenCloseAfterConnect()
        {
            const int port = 1337;

            using (var loop = new UvLoop())
            using (var server = loop.InitUvTcp())
            using (var client = loop.InitUvTcp())
            {
                UvException serverException = null;
                UvException clientException = null;
                var listenCallbackCalled = false;
                var connectCallbackCalled = false;

                server.Listen("0.0.0.0", port, 4,
                    (tcp, exception) =>
                        {
                            serverException = (UvException)exception;
                            tcp.Close();
                            listenCallbackCalled = true;
                        });
                client.Connect("127.0.0.1", port,
                    (tcp, exception) =>
                        {
                            clientException = (UvException)exception;
                            connectCallbackCalled = true;
                        });

                loop.Run();

                Assert.IsNull(serverException);
                Assert.IsNull(clientException);
                Assert.That(listenCallbackCalled);
                Assert.That(connectCallbackCalled);
            }
        }
    }
}