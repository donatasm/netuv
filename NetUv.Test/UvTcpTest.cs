using NUnit.Framework;

namespace NetUv.Test
{
    [TestFixture]
    internal sealed class UvTcpTest
    {
        [Test]
        public void ConnectThrowsUvException()
        {
            UvException uvException = null;
            UvTcp uvTcp = null;
            var tcpClosed = false;

            using (var loop = new UvLoop())
            {
                var client = loop.InitUvTcp();
                client.Connect("127.0.0.1", 1337,
                    (tcp, exception) =>
                        {
                            uvTcp = tcp;
                            uvException = (UvException)exception;
                            tcp.Close(handle =>
                                {
                                    tcpClosed = true;
                                    handle.Dispose();
                                });
                        });

                loop.Run();
            }

            Assert.IsNotNull(uvTcp);
            Assert.IsNotNull(uvException);
            Assert.AreEqual("ECONNREFUSED", uvException.Error);
            Assert.That(tcpClosed);
        }

        [Test]
        public void ListenCloseAfterConnect()
        {
            UvException serverException = null;
            UvException clientException = null;
            var listenCallbackCalled = false;
            var connectCallbackCalled = false;
            var serverCloseCallbackCalled = false;
            var clientCloseCallbackCalled = false;

            using (var loop = new UvLoop())
            {
                const int port = 1337;
                const int backlog = 1;

                var server = loop.InitUvTcp();
                server.Bind("0.0.0.0", port);
                server.Listen(backlog,
                    (tcp, exception) =>
                        {
                            serverException = (UvException)exception;
                            listenCallbackCalled = true;
                            tcp.Close(handle =>
                                          {
                                              serverCloseCallbackCalled = true;
                                              handle.Dispose();
                                          });
                        });

                var client = loop.InitUvTcp();
                client.Connect("127.0.0.1", port,
                    (tcp, exception) =>
                        {
                            clientException = (UvException)exception;
                            connectCallbackCalled = true;
                            tcp.Close(handle =>
                                          {
                                              clientCloseCallbackCalled = true;
                                              handle.Dispose();
                                          });
                        });

                loop.Run();
            }

            Assert.IsNull(serverException);
            Assert.IsNull(clientException);
            Assert.That(listenCallbackCalled);
            Assert.That(connectCallbackCalled);
            Assert.That(serverCloseCallbackCalled);
            Assert.That(clientCloseCallbackCalled);
        }
    }
}