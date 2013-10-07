using System.Diagnostics;
using System.Threading;
using NUnit.Framework;

namespace NetUv.Test
{
    [TestFixture]
    internal sealed class UvAsyncTest
    {
        private readonly UvLoop _loop;
        private readonly UvAsync _async;

        private const int AsyncSendCount = 10000;
        private int _asyncSendCalled;

        [Test]
        public void SendCallback()
        {
            new Thread(() =>
                {
                    for (var i = 0; i < AsyncSendCount; i++)
                    {
                        _async.Send();
                    }

                }).Start();

            _loop.Run();

            // async callback should be called at least once
            Assert.That(_asyncSendCalled > 0);
        }

        public UvAsyncTest()
        {
            _loop = new UvLoop();

            _async = (UvAsync)_loop.InitUvAsync((async, exception) =>
                {
                    _asyncSendCalled++;

                    Debug.WriteLine("Async callback: _asyncSendCalled={0}", _asyncSendCalled);

                    async.Close(handle => handle.Dispose());
                });
        }

        [TearDown]
        public void Dispose()
        {
            _loop.Dispose();
        }
    }
}
