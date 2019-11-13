using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Nekara.Networking
{
    public class WebSocketServer
    {
        private string host;
        private int port;
        private string path;
        public Dictionary<string, WebSocketClientHandle> clients;
        private HttpListener listener;
        private Action<WebSocketClientHandle> onNewClient;

        public WebSocketServer(string host, int port, string path = "")
        {
            this.host = host;
            this.port = port;
            this.path = path;
            this.clients = new Dictionary<string, WebSocketClientHandle>();
            this.listener = new HttpListener();
            this.listener.Prefixes.Add("http://" + host + ":" + port.ToString() + "/" + path);
            this.onNewClient = (WebSocketClientHandle client) => { };
        }

        // Listen returns immediately, and the "listening" is done asynchronously via Task loop.
        public void Listen()
        {
            listener.Start();
            Console.WriteLine("WebSocketServer Listening...");

            Helpers.AsyncLoop(HandleRequest);
        }

        private void HandleRequest()
        {
            HttpListenerContext context = listener.GetContext();

            // First check if this is a websocket handshake
            if (context.Request.IsWebSocketRequest)
            {
                Console.WriteLine("  --> WebSocket Request Received!!!");
                context.AcceptWebSocketAsync(null)
                    .ContinueWith(prev => this.AddClient(prev.Result));
            }
        }

        public void OnNewClient (Action<WebSocketClientHandle> handler)
        {
            this.onNewClient = handler;
        }

        // This method uses an UNDOCUMENTED object - ServerWebSocket
        public WebSocketClientHandle AddClient(HttpListenerWebSocketContext context)
        {
            var client = new WebSocketClientHandle(this, context.WebSocket);
            this.clients.Add(client.id, client);
            client.OnClose(() => { this.RemoveClient(client.id); });
            this.onNewClient(client);
            return client;
        }

        public WebSocketClientHandle GetClient(string clientId)
        {
            return this.clients[clientId];
        }

        public void RemoveClient(string socketId)
        {
            // var client = this.clients[socketId];
            // client.socket.Dispose();
            // client.CloseAsync(WebSocketCloseStatus.NormalClosure, "Close OK", CancellationToken.None)
            //.ContinueWith(prev => client.Dispose());
            this.clients.Remove(socketId);
        }
    }

    public class WebSocketClientHandle
    {
        public readonly string id;
        public readonly WebSocket socket;
        private AsyncLock sendLock;
        private Action onClose;
        private Action<string> onMessage;

        public WebSocketClientHandle(WebSocketServer server, WebSocket socket)
        {
            this.id = Helpers.RandomString(16);
            this.socket = socket;
            this.sendLock = AsyncLock.Create("WebSocketClientHandle-" + this.id);
            this.onClose = () => { };

            var socketDestroyer = new CancellationTokenSource();
            Console.WriteLine("  --> New WebSocketClient {0}", this.id);

            byte[] buffer = new byte[65536]; // 8 KB buffer
            Helpers.AsyncTaskLoop(() =>
            {
                if (socket.State == WebSocketState.Open)
                {
                    return socket.ReceiveAsync(new ArraySegment<byte>(buffer), socketDestroyer.Token)
                    .ContinueWith(prev =>
                    {
                        try
                        {
                            var received = prev.Result;
                            // Console.WriteLine("WebSocket {0}: {1} {2} {3}", this.id, received.Count, received.MessageType, received.EndOfMessage);
                            if (prev.Result.MessageType == WebSocketMessageType.Close)
                            {
                                Console.WriteLine("  !!! WebSocket Client {0} dropped connection", this.id);
                                Console.WriteLine("  !!! Closing WebSocket {0}", this.id);
                                socketDestroyer.Cancel();

                                // return Task.CompletedTask;
                                return socket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Close OK", CancellationToken.None)
                                    .ContinueWith(_ =>
                                    {
                                        this.onClose();
                                        this.socket.Dispose();
                                    });
                            }
                            else
                            {
                                string message = Encoding.UTF8.GetString(buffer, 0, prev.Result.Count);
                                // Console.WriteLine("  !!! Message from WebSocket {0}: {1}", this.id, message);
                                
                                // Spawning a new task to make the message handler "non-blocking"
                                // TODO: Errors thrown inside here will become silent, so that needs to be handled
                                // Also, now that the single execution flow is broken, the requests are under race conditions
                                return Task.Run(() => this.onMessage(message));
                            }
                        }
                        catch (AggregateException ae)
                        {
                            Console.WriteLine("  !!! Exception during async communication with Client {0}", this.id);
                            Console.WriteLine("  ... Connection closed... if this was not expected, inspect the AggregateException here");
                            foreach(var ie in ae.Flatten().InnerExceptions)
                            {
                                Console.WriteLine("Exception -------------------");
                                Console.WriteLine(ie.Message);
                                Console.WriteLine(ie.InnerException.Message);
                                Console.WriteLine(ie.InnerException.StackTrace);
                                Console.WriteLine("-----------------------------\n");
                            }
                            socketDestroyer.Cancel();
                            this.onClose();
                            this.socket.Dispose();
                            return Task.CompletedTask;
                        }
                    }).Unwrap();
                }
                else
                {
                    Console.WriteLine("  !!! WebSocket {0} Connection Dropped!", this.id);
                    socketDestroyer.Cancel();
                    this.onClose();
                    this.socket.Dispose();
                    return Task.CompletedTask;
                }
            }, socketDestroyer.Token);

        }

        public void OnClose(Action handler)
        {
            this.onClose = handler;
        }

        public void OnMessage(Action<string> handler)
        {
            this.onMessage = handler;
        }

        public Task Send(string payload)
        {
            byte[] buffer = Encoding.UTF8.GetBytes(payload);
            // We lock the socket because multiple tasks can be racing to use the websocket.
            // The websocket will fail if two tasks try to call client.Send concurrently.
            // We use the low-level Monitor.Enter/Exit because we need to release asynchronously

            var releaser = this.sendLock.Acquire().Result;
            var sendTask = socket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Text, true, CancellationToken.None);
            sendTask.ContinueWith(t => releaser.Dispose());
            return sendTask;
        }
    }
}
