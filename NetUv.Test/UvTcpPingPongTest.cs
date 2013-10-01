using System;
using System.Diagnostics;
using System.Text;
using NUnit.Framework;

namespace NetUv.Test
{
    [TestFixture]
    internal sealed class UvTcpPingPongTest
    {
        private const int Port = 1337;
        private const int Count = 10000; // how many ping-pongs to do
        private static readonly byte[] Ping = Encoding.ASCII.GetBytes("PING");
        private static readonly byte[] Pong = Encoding.ASCII.GetBytes("PONG");

        // all pings
        private static readonly byte[] Pings = new byte[Count * Ping.Length];

        // all pongs
        private static readonly byte[] Pongs = new byte[Count * Pong.Length];

        private UvTcp _server;
        private UvTcp _accept;
        private UvTcp _client;
        private UvLoop _loop;

        private int _currentCount;
        private int _serverClose;
        private int _acceptClose;
        private int _clientClose;

        [Test]
        public void PingPong()
        {
            _server = _loop.InitUvTcp();
            _server.Bind("0.0.0.0", Port);
            _server.Listen(1, ListenCb);

            _client = _loop.InitUvTcp();
            _client.Connect("127.0.0.1", Port, ConnectCb);

            _loop.Run();

            Assert.AreEqual(Count, _currentCount);
            Assert.AreEqual(1, _serverClose);
            Assert.AreEqual(1, _acceptClose);
            Assert.AreEqual(1, _clientClose);
        }

        private void ListenCb(UvStream server, Exception exception)
        {
            Debug.WriteLine("[Server]: ListenCb");

            AssertNoException(server, exception);

            _accept = _loop.InitUvTcp();
            server.Accept(_accept);

            Debug.WriteLine("[Server]: ReadStart Ping");
            _accept.ReadStart(PingAllocCb, PingReadCb);
        }

        private UvBuffer PingAllocCb(int suggestedbuffersize)
        {
            var offset = _currentCount * Ping.Length;
            var count = Ping.Length;

            Debug.WriteLine("[Server]: PingAllocCb offset={0} count={1}",
                offset, count);

            return new UvBuffer(Pings, offset, count);
        }

        private void PingReadCb(UvStream server, int read, UvBuffer buffer)
        {
            Debug.WriteLine("[Server]: PingReadCb read={0}", read);

            if (read < 0)
            {
                Debug.WriteLine("[Server]: Close");
                server.Close(handle =>
                                {
                                    handle.Dispose();
                                    Assert.Fail("Ping read failed.");
                                });
            }

            if (read == 0)
            {
                return;
            }

            if (_currentCount < Count)
            {
                Debug.WriteLine("[Server]: Write Pong");
                server.Write(new UvBuffer(Pong, 0, Pong.Length), PongWriteCb);
            }
            else
            {
                Debug.WriteLine("[Server]: Close");
                server.Close(handle =>
                                 {
                                     handle.Dispose();
                                     _acceptClose++;
                                 });
                _server.Close(handle =>
                                  {
                                      handle.Dispose();
                                      _serverClose++;
                                  });
            }
        }

        private static void PongWriteCb(UvStream server, Exception exception)
        {
            Debug.WriteLine("[Server]: PongWriteCb");

            AssertNoException(server, exception);
        }

        private void ConnectCb(UvTcp client, Exception exception)
        {
            Debug.WriteLine("[Client]: ConnectCb");

            AssertNoException(client, exception);

            Debug.WriteLine("[Client]: Write Ping");
            client.Write(new UvBuffer(Ping, 0, Ping.Length), PingWriteCb);
        }

        private void PingWriteCb(UvStream client, Exception exception)
        {
            Debug.WriteLine("[Client]: PingWriteCb");

            AssertNoException(client, exception);

            Debug.WriteLine("[Client]: ReadStart Pong");
            client.ReadStart(PongAllocCb, PongReadCb);
        }

        private UvBuffer PongAllocCb(int suggestedBufferSize)
        {
            var offset = _currentCount * Pong.Length;
            var count = Pong.Length;

            Debug.WriteLine("[Client]: PongAllocCb offset={0} count={1}",
                offset, count);

            return new UvBuffer(Pongs, offset, count);
        }

        private void PongReadCb(UvStream client, int read, UvBuffer buffer)
        {
            Debug.WriteLine("[Client]: PongReadCb read={0}", read);

            if (read < 0)
            {
                client.Close(handle =>
                                {
                                    handle.Dispose();
                                    _clientClose++;

                                    if (read != UvStream.EOF)
                                    {
                                        Assert.Fail("Pong read failed.");
                                    }
                                });
                return;
            }

            if (read == 0)
            {
                return;
            }

            if (_currentCount < Count)
            {
                client.Write(new UvBuffer(Pong, 0, Pong.Length), PongWriteCb);
            }
            else
            {
                client.Close(handle => handle.Dispose());
            }

            _currentCount++;
        }

        private static void AssertNoException(UvHandle handle, Exception exception)
        {
            if (exception != null)
            {
                handle.Close(h =>
                    {
                        h.Dispose();
                        Assert.Fail(exception.ToString());
                    });
            }
        }

        [SetUp]
#pragma warning disable 628
        protected void InitLoop()
#pragma warning restore 628
        {
            _loop = new UvLoop();
        }

        [TearDown]
#pragma warning disable 628
        protected void DisposeLoop()
#pragma warning restore 628
        {
            _loop.Dispose();
        }
    }
}
