using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
//using System.Runtime.Remoting;
//using System.Runtime.Remoting.Channels;
//using System.Runtime.Remoting.Channels.Ipc;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Reflection;
using System.Diagnostics;

namespace Nekara.Networking
{
    // This is the main "host" object that is created by the server process.
    // It does not contain the testing logic - that resides in the TestingService
    // (the actual tester should perhaps call into the existing psharp tester)
    // The transport mechanism is transparent, and this object only exposes the
    // high-level, protocol-agnostic interface.
    public class OmniServer
    {
        private OmniServerConfiguration config;
        private Dictionary<string, RemoteMethodAsync> remoteMethods;
        private Func<WebSocketClientHandle> _getClient;    // delegate method for getting client handles - this is a workaround to handle the InitializeTestSession notifyClient callback.

        public OmniServer(OmniServerConfiguration config)
        {
            this.config = config;
            this.remoteMethods = new Dictionary<string, RemoteMethodAsync>();

            // for debugging
            this.remoteMethods.Add("echo", (senderId, args) => Task<JToken>.FromResult(JToken.FromObject(args)));

            // Initialize the testing service before setting up the transport
            // (if it is IPC, it will be initialized differently)

            /*if (this.config.transport != Transport.IPC)
            {
                this.service = new TestingService();
                RegisterRemoteMethods(this.service);
            }*/

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
                default: throw new TesterConfigurationException("Unknown Transport Protocol");
            }
        }

        // for registering message handlers that are methods of objects (these method are probably stateful)
        private void RegisterRemoteMethod(string name, object instance, MethodInfo method)
        {
            if (method.ReturnType == typeof(Task<JToken>))
            {
                this.remoteMethods[name] = (senderId, args) =>
                {
                    // Console.WriteLine("    Invoking Remote Method: Task<JToken> {0} ({1})", name, String.Join(",", args.Select(t => t.ToString())));
                    return (Task<JToken>)method.Invoke(instance, args);
                };
            }
            else if (method.ReturnType == typeof(void))
            {
                this.remoteMethods[name] = (senderId, args) =>
                {
                    try
                    {
                        // Console.WriteLine("    Invoking Remote Method: void {0} ({1})", name, String.Join(",", args.Select(t => t.ToString())) );
                        method.Invoke(instance, args);
                        // method.Invoke(instance, new object[] { args[0].ToObject<int>() });
                        return Task<JToken>.FromResult(JToken.FromObject(0));
                    }
                    catch (TargetInvocationException ex)
                    {
                        Console.WriteLine("[OmniServer.RegisterRemoteMethod] Caught TargetInvocationException/{0}!", ex.InnerException.GetType().Name);
                        if (ex.InnerException is AmbiguousMatchException
                            || ex.InnerException is InvalidOperationException
                            || ex.InnerException is TargetInvocationException
                            || ex.InnerException is AggregateException)
                        {
                            Console.WriteLine(ex.InnerException);
                        }
                        throw ex.InnerException;
                    }
                };
            }
            else
            {
                this.remoteMethods[name] = (senderId, args) =>
                {
                    // Console.WriteLine("    Invoking Remote Method: {0} ({1})", name, String.Join(",", args.Select(t => t.ToString())));
                    try
                    {
                        object result = method.Invoke(instance, args);

                        return Task<JToken>.FromResult(JToken.FromObject(result));
                    }
                    catch (TargetInvocationException ex)
                    {
                        Console.WriteLine("[OmniServer.RegisterRemoteMethod] Caught TargetInvocationException/{0}!", ex.InnerException.GetType().Name);
                        if (ex.InnerException is AmbiguousMatchException
                            || ex.InnerException is InvalidOperationException
                            || ex.InnerException is TargetInvocationException
                            || ex.InnerException is AggregateException)
                        {
                            Console.WriteLine(ex.InnerException);
                        }
                        throw ex.InnerException;
                    }
                };
            }
            Console.WriteLine("    Registered Remote Method: {0}.{1}", instance.GetType().Name, name);
        }

        // for registering message handlers that are lambda functions (these method are probably stateless)
        public void RegisterRemoteMethod(string name, Func<object, Task<JToken>> handler)
        {
            // wrapping it to print some info
            // it can actually be directly assigned like: new RemoteMethodAsync(handler);
            RemoteMethodAsync method = new RemoteMethodAsync((senderId, args) => {
                Console.WriteLine("    Invoking Stateless Remote Method {0} ({1})", name, args);
                return handler(args);
            });
            this.remoteMethods[name] = method;
            Console.WriteLine("    Registered Remote Method: {0}", name);
        }

        // for registering a "service" object - i.e., has methods decorated with RemoteMethodAttribute
        public void RegisterService(object service)
        {
            // register the test service API
            var methods = service.GetType().GetMethods()
                    .Where(m => m.GetCustomAttributes(false).OfType<RemoteMethodAttribute>().Any())
                    .ToDictionary(a => a.Name);
            foreach (var item in methods)
            {
                this.RegisterRemoteMethod(item.Key, service, item.Value);
            }
        }

        public WebSocketClientHandle GetClient()
        {
            return this._getClient();
        }

        // this method is called internally by the main message listener loop
        private Task<ResponseMessage> HandleRequest(RequestMessage message)
        {
            // Console.WriteLine("--> Client Request {2}:  {0} ({1})", message.func, String.Join(",", message.args.Select(x => x.ToString()) ), message.id);
            if (this.remoteMethods.ContainsKey(message.func))
            {
                // If any Exception thrown during remoteMethod invocation,
                // forward it to the client instead of failing on the server-side
                try
                {
                    // Spawning a new task to make the message handler "non-blocking"
                    // TODO: Errors thrown inside here will become silent, so that needs to be handled
                    // Also, now that the single execution flow is broken, the requests are under race conditions
                    return Task.Factory.StartNew<Task<JToken>>(() => this.remoteMethods[message.func](message.sender, message.args.ToArray<JToken>())).Unwrap()
                    // return this.remoteMethods[message.func](message.args.ToArray())
                    .ContinueWith(prev => {
                        // Console.WriteLine("    ... responding to {2}:  {0} {1}", message.func, prev.IsFaulted, message.id);
                        if (prev.IsFaulted)
                        {
                            // Console.WriteLine("[OmniServer.HandleRequest] {0} Caught while handling [{1}]!", prev.Exception.GetType().Name, message.func);
                            // Console.WriteLine(prev.Exception);
                            if (prev.Exception is AggregateException)
                            {
                                return message.CreateErrorResponse("Tester-Server", JToken.FromObject(prev.Exception.Flatten().InnerException));
                            }
                            else return message.CreateErrorResponse("Tester-Server", JToken.FromObject(prev.Exception));
                        }
                        if (prev.Result != null) return message.CreateResponse("Tester-Server", prev.Result);
                        return message.CreateResponse("Tester-Server", new JValue("OK"));
                    });
                }
                catch (Exception ex)
                {
                    Console.WriteLine("!!! {0} Caught while invoking [{1}]", ex.GetType().Name, message.func);
                    Console.WriteLine(ex);
                    // Console.WriteLine(ex);
                    return Task.FromResult(message.CreateErrorResponse("Tester-Server", JToken.FromObject(ex)));
                }
            }
            else
            {
                Console.WriteLine("!!! Client Requesting non-existent remote function [{0}]", message.func);
                // return Task.FromResult(message.CreateErrorResponse("Tester-Server", new JValue("ERROR: Could not understand func " + message.func)));
                return Task.FromResult(message.CreateErrorResponse("Tester-Server", JToken.FromObject(new RemoteMethodDoesNotExistException("ERROR: Could not understand func " + message.func))));
            }
        }

        private void SetupTransportIPC()
        {
            throw new NotImplementedException();
/*
            // Create and register the IPC channel
            IpcServerChannel channel = new IpcServerChannel("tester");
            ChannelServices.RegisterChannel(channel, false);

            // Expose an object -- an interface to the service
            RemotingConfiguration.RegisterWellKnownServiceType(typeof(NekaraServer), "service", WellKnownObjectMode.Singleton);

            // Wait for calls
            Console.WriteLine("... Tester Server Listening on IPC Channel: " + channel.GetChannelUri());
*/
        }

        private void SetupTransportHTTP()
        {
            // Create an HttpServer and bind to network socket
            HttpServer server = new HttpServer(this.config.host, this.config.port);

            // Top-level middleware function - just print some things for debugging
            server.Use((Request request, Response response, Action next) => {
                Console.WriteLine("Received {0} {1}", request.method, request.path);
                Console.WriteLine(request.body);
                next();
            });

            // test endpoint
            server.Post("echo/", (Request request, Response response, Action next) =>
            {
                response.Send(200, request.body);
            });

            /* Expose the service */
            server.Post("rpc/", (Request request, Response response, Action next) =>
            {
                RequestMessage message = JsonConvert.DeserializeObject<RequestMessage>(request.body);
                HandleRequest(message)
                .ContinueWith(prev =>
                {
                    //Console.WriteLine("<-- Returning {3}Response to {2}:  {0} ({1})", message.func, String.Join(",", message.args.Select(x => x.ToString())), message.id, prev.Result.error ? "Error " : "");
                    ResponseMessage reply = prev.Result;
                    response.Send(200, reply);
                });
            });

            server.Listen();

            // Wait for calls
            Console.WriteLine("... Tester Server Listening on HTTP Host: {0}, Port: {1}", this.config.host, this.config.port);
        }

        private void SetupTransportGRPC()
        {
            throw new NotImplementedException();
        }

        private void SetupTransportWS()
        {
            // Create a WebSocket Server
            WebSocketServer server = new WebSocketServer("localhost", 8080, "ws/");

            this._getClient = () =>
            {
                string clientId = server.clients.Keys.First();
                return server.GetClient(clientId);
            };

            /* Expose the service */
            server.OnNewClient((WebSocketClientHandle client) =>
            {
                client.OnMessage((string data) => {

                    RequestMessage message = RequestMessage.Deserialize(data);

                    HandleRequest(message)
                    .ContinueWith(prev =>
                    {
                        //Console.WriteLine("<-- Returning {3}Response to {2}:  {0} ({1})", message.func, String.Join(",", message.args.Select(x => x.ToString())), message.id, prev.Result.error ? "Error " : "");
                        ResponseMessage reply = prev.Result;

                        client.Send(reply.Serialize());
                    });

                });
            });

            server.Listen();

            // Wait for calls
            Console.WriteLine("... WebSocket Server Listening on HTTP Port: 8080");
        }

        private void SetupTransportTCP()
        {
            throw new NotImplementedException();
        }
    }
}
