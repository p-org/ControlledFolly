using System;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Net.WebSockets;

namespace Nekara.Networking
{
    // Wrapping the native ClientWebSocket class to provide a different high-level interface
    class WebSocketClient : JsonP2P, IDisposable
    {
        private string serverUri;
        private ClientWebSocket socket;
        public event Action<string> onMessage;
        private AsyncLock sendLock;
        // private object sendLock;
        private object receiveLock;
        public readonly Task ReadyFlag;

        public WebSocketClient(string serverUri) : base()
        {
            this.serverUri = serverUri;
            this.socket = new ClientWebSocket();
            this.sendLock = AsyncLock.Create("WebSocketClient");
            // this.sendLock = new object();
            this.receiveLock = new object();

            this.ReadyFlag = this.socket.ConnectAsync(new Uri(this.serverUri), CancellationToken.None)
                .ContinueWith(prev =>
                {
                    Listen();
                });
        }

        private void Listen()
        {
            var socketDestroyer = new CancellationTokenSource();
            var buffer = new byte[65536];
            Helpers.AsyncTaskLoop(() =>
            {
                /*try
                {
                    Monitor.Enter(receiveLock);*/
                    if (this.socket.State == WebSocketState.Open)
                    {
                        return this.socket.ReceiveAsync(new ArraySegment<byte>(buffer), socketDestroyer.Token)
                            .ContinueWith(prev =>
                            {
                                //Console.WriteLine("WebSocket received {0} bytes of {1} data, end of message = {2}", prev.Result.Count, prev.Result.MessageType, prev.Result.EndOfMessage);

                                if (prev.Result.MessageType == WebSocketMessageType.Text)
                                {
                                    string payload = Encoding.UTF8.GetString(buffer, 0, prev.Result.Count);

                                    // Spawning a new task to make the message handler "non-blocking"
                                    // TODO: Errors thrown inside here will become silent, so that needs to be handled
                                    // Also, now that the single execution flow is broken, the requests are under race conditions
                                    Task.Run(() => {
                                        // Console.WriteLine("  ... handling message on thread {0}", Thread.CurrentThread.ManagedThreadId);
                                        try
                                        {
                                            this.HandleMessage(payload);

                                            if (this.onMessage != null)
                                            {
                                                this.onMessage(payload);
                                            }
                                        }
                                        catch (UnexpectedMessageException ex)
                                        {
                                            Console.WriteLine(ex);
                                        }
                                        catch (AggregateException ae)
                                        {
                                            // Console.WriteLine(ae);
                                            foreach (var ie in ae.Flatten().InnerExceptions)
                                            {
                                                Console.WriteLine("Exception -------------------");
                                                Console.WriteLine(ie.Message);
                                                Console.WriteLine(ie.InnerException.Message);
                                                Console.WriteLine(ie.InnerException.StackTrace);
                                                Console.WriteLine("-----------------------------\n");
                                            }

                                            ae.Handle(e =>
                                            {
                                                if (e is WebSocketException)
                                                {
                                                    Console.WriteLine("!!! WebSocketException - Connection Closed");
                                                    Console.WriteLine("!!! If this was unexpected, inspect the exception object here");
                                                    socketDestroyer.Cancel();
                                                    return true;
                                                }
                                                else
                                                {
                                                    Console.WriteLine("!!! Unexpected Exception: {0}", e);
                                                    socketDestroyer.Cancel();
                                                    return false;
                                                }
                                            });
                                        }
                                        catch (Exception ex)
                                        {
                                            Console.WriteLine("[WebSocketClient.Listen] ! Exception caught while handling payload");
                                            Console.WriteLine(ex);
                                            throw;
                                        }
                                    });
                                }
                                else if (prev.Result.MessageType == WebSocketMessageType.Close)
                                {
                                    Console.WriteLine("  !!! WebSocket Server closed connection");
                                    Console.WriteLine("  !!! Closing WebSocket");
                                    socketDestroyer.Cancel();
                                }
                            });
                    }
                    else
                    {
                        Console.WriteLine("!!! WebSocket Connection Dropped!");
                        socketDestroyer.Cancel();
                        return Task.CompletedTask;
                    }
                /*}
                finally
                {
                    Monitor.Exit(receiveLock);
                }*/
            }, socketDestroyer.Token);
        }

        public override Task Send(string recipient, string payload)
        {
            byte[] buffer = Encoding.UTF8.GetBytes(payload);
            
            // We lock the socket because multiple tasks can be racing to use the websocket.
            // The websocket will fail if two tasks try to call client.Send concurrently.
            var releaser = this.sendLock.Acquire().Result;

            // It is important that we use the default Task scheduler
            // to make this lock work correctly with arbitrary frameworks.
            // For instance, without specifying the default task scheduler, this lock fails in orleans,
            // as described in: http://dotnet.github.io/orleans/Documentation/grains/external_tasks_and_grains.html
            // The orleans task scheduler will enforce a single-threaded execution model and the lock will deadlock against the async IO operation

            var sendTask = this.socket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Text, true, CancellationToken.None);
            sendTask.ContinueWith(t => {
                releaser.Dispose(); 
            }, TaskScheduler.Default);

            return sendTask;
        }

        public void Dispose()
        {
            this.socket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Client Signing Out", CancellationToken.None).Wait();
            this.socket.Dispose();
        }
    }
}
