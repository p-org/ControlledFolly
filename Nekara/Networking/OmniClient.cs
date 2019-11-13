using System;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
//using System.Runtime.Remoting;
//using System.Runtime.Remoting.Channels;
//using System.Runtime.Remoting.Channels.Ipc;
using Newtonsoft.Json.Linq;

namespace Nekara.Networking
{
    public class OmniClient : IClient, IDisposable
    {
        private OmniClientConfiguration config;
        private Func<string, JToken[], (Task<JToken>, CancellationTokenSource)> _sendRequest;    // delegate method to be implemented by differnet transport mechanisms
        private Action<string, RemoteMethodAsync> _addRemoteMethod;    // delegate method to be implemented by differnet transport mechanisms
        private Action _dispose;
        private Task _readyFlag;

        public Task ReadyFlag { get { return _readyFlag; } }
        
        // private Func<string, Task> Subscribe;            // using topic-based Publish-Subscribe
        // private Func<string, string, Task> Publish;      // using topic-based Publish-Subscribe

        public OmniClient(OmniClientConfiguration config)
        {
            this.config = config;

            // Depending on the transport, create the appropriate communication interface
            switch (this.config.Transport)
            {
                case Transport.IPC:
                    SetupTransportIPC();
                    break;
                case Transport.HTTP:
                    SetupTransportHTTP();
                    break;
                case Transport.GRPC:
                    SetupTransportGRPC();
                    break;
                case Transport.WS:
                    SetupTransportWS();
                    break;
                case Transport.TCP:
                    SetupTransportTCP();
                    break;
                default: throw new Exception(); // TODO: make a proper exception later
            }

            // __testStart();
        }

        private void SetupTransportIPC()
        {
            throw new NotImplementedException();
/*

            // Create and register the IPC channel
            IpcClientChannel channel = new IpcClientChannel();
            ChannelServices.RegisterChannel(channel, false);

            // Fetch the proxy object -- an interface to the service
            RemotingConfiguration.RegisterWellKnownClientType(typeof(NekaraServer), "ipc://tester/service");

            // TestingService service = new TestingService();

            // Assign the appropriate SendRequest method
            this._sendRequest = (func, args) =>
            {
                // TODO: this function is incomplete - work on it later
                return ( Task.FromResult(JValue.Parse("true")), new CancellationTokenSource() );
            };

            this._addRemoteMethod = (string func, RemoteMethodAsync handler) =>
            {

            };

            this._dispose = () => {
                
            };
*/
        }

        private void SetupTransportHTTP()
        {
            HttpClient client = new HttpClient("http://" + this.config.serviceHost + ":" + this.config.servicePort.ToString() + "/");

            // Assign the appropriate SendRequest method
            this._sendRequest = (func, args) =>
            {
                var tcs = new TaskCompletionSource<JToken>();
                var cts = new CancellationTokenSource();
                client.Post("rpc/", new RequestMessage("Tester-Client", "Tester-Server", func, args).Serialize())
                    .ContinueWith(prev => {
                        var resp = ResponseMessage.Deserialize(prev.Result);
                        if (resp.error) tcs.SetException(Exceptions.DeserializeServerSideException(resp.data));
                        else tcs.SetResult(resp.data);
                    });
                return (tcs.Task, cts);
            };

            this._addRemoteMethod = (string func, RemoteMethodAsync handler) =>
            {

            };

            this._dispose = () => {
                client.Dispose();
            };

            this._readyFlag = client.ReadyFlag;
        }

        private void SetupTransportGRPC()
        {
            throw new NotImplementedException();
        }

        private void SetupTransportWS()
        {
            WebSocketClient client = new WebSocketClient("ws://localhost:8080/ws/");
            /*client.onMessage += (string data) =>
            {
                Console.WriteLine("WSC.onMessage triggered: {0}", data);
            };*/

            // Assign the appropriate SendRequest method
            this._sendRequest = (string func, JToken[] args) => client.Request("Tester-Server", func, args);

            this._addRemoteMethod = (string func, RemoteMethodAsync handler) => client.RegisterRemoteMethod(func, handler);

            this._dispose = () => {
                client.Dispose();
            };

            this._readyFlag = client.ReadyFlag;
        }

        private void SetupTransportTCP()
        {
            throw new NotImplementedException();
        }

        // overloading the main SendRequest method to deal with variadic arguments
        public (Task<JToken>, CancellationTokenSource) SendRequest(string func)
        {
            return this._sendRequest(func, new JToken[] { });
        }

        public (Task<JToken>, CancellationTokenSource) SendRequest(string func, JArray args)
        {
            return this._sendRequest(func, args.ToArray<JToken>());
        }

        public (Task<JToken>, CancellationTokenSource) SendRequest(string func, params JToken[] args)
        {
            return this._sendRequest(func, args);
        }

        public (Task<JToken>, CancellationTokenSource) SendRequest(string func, params bool[] args)
        {
            return this._sendRequest(func, args.Select(x => JToken.FromObject(x)).ToArray());
        }

        public (Task<JToken>, CancellationTokenSource) SendRequest(string func, params int[] args)
        {
            return this._sendRequest(func, args.Select(x => JToken.FromObject(x)).ToArray());
        }

        public (Task<JToken>, CancellationTokenSource) SendRequest(string func, params string[] args)
        {
            return this._sendRequest(func, args.Select(x => JToken.FromObject(x)).ToArray());
        }

        public void AddRemoteMethod(string func, RemoteMethodAsync handler)
        {
            this._addRemoteMethod(func, handler);
        }

        // Using this method only during the early stages of development
        // Will be removed after everything is setup
        private void __testStart()
        {

            Helpers.AsyncTaskLoop(() =>
            {
                Console.Write("HTTP: ");
                string input = Console.ReadLine();
                input = Regex.Replace(input, @"[ \t]+", " ");

                string[] tokens = input.Split(' ');
                if (tokens.Length > 0)
                {
                    string cmd = tokens[0].ToLower();
                    if (cmd == "exit" || cmd == "quit") Environment.Exit(0);
                    else if (tokens.Length > 2)
                    {
                        if (cmd == "echo")
                        {
                            var (t, c) = this._sendRequest(cmd, tokens.Skip(1).Select(x => JToken.FromObject(x)).ToArray());
                            return t;
                        }
                        else if (cmd == "do")
                        {
                            string func = tokens[1];
                            JToken[] args = tokens.Skip(2).Select(x => JToken.FromObject(x)).ToArray();
                            var (t, c) = this._sendRequest(func, args);
                            return t;
                        }
                    }
                }

                return Task.Run(() => { });
            });

            // block the main thread here to prevent exiting - as AsyncTaskLoop will return immediately
            while (true)
            {
            }
        }

        public void Dispose()
        {
            this._dispose();
        }
    }
}
