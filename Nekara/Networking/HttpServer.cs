using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using Newtonsoft.Json;

/*
 * TODO:
 *   - Need Sessions (Cookie-based for HTTP): cannot currently distinguish between different requests
 */

namespace Nekara.Networking
{
    delegate void Middleware(Request request, Response response, Action next);

    class Router
    {
        private protected List<Middleware> middlewares;
        private protected Dictionary<string, List<Middleware>> methodMiddlewares;
        private protected Dictionary<string, Router> routeMap;
        
        public Router()
        {
            this.middlewares = new List<Middleware>();
            this.methodMiddlewares = new Dictionary<string, List<Middleware>>();
            this.methodMiddlewares.Add("OPTIONS", new List<Middleware>());
            this.methodMiddlewares.Add("GET", new List<Middleware>());
            this.methodMiddlewares.Add("POST", new List<Middleware>());
            this.methodMiddlewares.Add("PUT", new List<Middleware>());
            this.methodMiddlewares.Add("DELETE", new List<Middleware>());
            this.routeMap = new Dictionary<string, Router>();
        }

        public Router Route(string path)
        {
            Router router;
            if (this.routeMap.TryGetValue(path, out router))
            {
                return router;
            }
            else
            {
                router = new Router();
                this.routeMap.Add(path, router);
                Console.WriteLine("    Added new Router at: {0}", path);
                return router;
            }
        }

        public void Use(Middleware middleware)
        {
            this.middlewares.Add(middleware);
        }

        public void Use(string path, Middleware middleware)
        {
            Router router = this.Route(path);
            router.Use(middleware);
            // this.middlewares.Add(middleware);
            Console.WriteLine("    Added new middleware at: {0}", path);
        }

        protected void UseOnMethod(string method, Middleware middleware)
        {
            this.methodMiddlewares[method].Add(middleware);
        }

        public void Options(string path, Middleware middleware)
        {
            Router router = this.Route(path);
            router.UseOnMethod("OPTIONS", middleware);
        }

        public void Get(string path, Middleware middleware)
        {
            Router router = this.Route(path);
            router.UseOnMethod("GET", middleware);
        }

        public void Post(string path, Middleware middleware)
        {
            Router router = this.Route(path);
            router.UseOnMethod("POST", middleware);
        }

        public void Put(string path, Middleware middleware)
        {
            Router router = this.Route(path);
            router.UseOnMethod("PUT", middleware);
        }

        public void Delete(string path, Middleware middleware)
        {
            Router router = this.Route(path);
            router.UseOnMethod("DELETE", middleware);
        }

        protected void HandleRequest(string[] pathSegments, Request request, Response response)
        {
            if (pathSegments.Length == 0)
            {
                Action next = null;
                int index = 0;          // these variables are captured by "next" closure
                int methodIndex = 0;    // these variables are captured by "next" closure
                next = () => {
                    if (index < this.middlewares.Count)
                    {
                        Middleware handler = this.middlewares[index];
                        index++;
                        handler(request, response, next);
                    }
                    else if (methodIndex < this.methodMiddlewares[request.method].Count)
                    {
                        Middleware handler = this.methodMiddlewares[request.method][methodIndex];
                        methodIndex++;
                        handler(request, response, next);
                    }
                };
                next();
            }
            else
            {
                Router router = this.routeMap[pathSegments[0]];
                router.HandleRequest(pathSegments.Skip(1).ToArray(), request, response);
            }
        }
    }

    // light wrapper around the native HttpListenerRequest class to hide away the low-level details
    // this may not be necessary as the native class is pretty high-level, but keeping it for now in case we need that extra level of indirection.
    class Request
    {
        private HttpListenerRequest request;
        private readonly string _body;

        public Request(HttpListenerRequest request)
        {
            this.request = request;

            //Console.WriteLine("HTTP: {0} {1}", request.HttpMethod, request.Url.PathAndQuery);

            if (request.HasEntityBody)
            {
                Stream stream = request.InputStream;
                Encoding encoding = request.ContentEncoding;
                StreamReader reader = new StreamReader(stream, encoding);

                /*if (request.ContentType != null)
                {
                    Console.WriteLine("Client data content type {0}", request.ContentType);
                }
                Console.WriteLine("Client data content length {0}", request.ContentLength64);*/

                // Print some meta info
                /*if (request.ContentType != null)
                {
                    Console.WriteLine("    Content-Type: {0}", request.ContentType);
                    Console.WriteLine("    Payload Size: {0}", request.ContentLength64);
                }*/

                // Console.WriteLine("Start of client data:");
                // Convert the data to a string and display it on the console.
                this._body = reader.ReadToEnd();

                //Console.WriteLine(this._body);
                // Console.WriteLine("End of client data:");
                stream.Close();
                reader.Close();
            }
            else this._body = "";
        }

        public string method { get { return this.request.HttpMethod; } }
        public string path { get { return this.request.Url.PathAndQuery; } }
        public string[] pathSegments { get { return this.request.Url.Segments; } }
        public string body { get { return this._body; } }
        public CookieCollection cookies { get { return this.request.Cookies; } }
    }

    // light wrapper around the native HttpListenerResponse class to hide away the low-level details
    class Response
    {
        private HttpListenerResponse response;

        public Response(HttpListenerResponse response)
        {
            this.response = response;
        }

        public void Send(int statusCode, string payload)
        {
            // Construct a response.
            // string responseString = "<HTML><BODY> Hello world!</BODY></HTML>";
            byte[] buffer = System.Text.Encoding.UTF8.GetBytes(payload);

            // Get a response stream and write the response to it.
            this.response.ContentLength64 = buffer.Length;
            this.response.StatusCode = statusCode;
            System.IO.Stream output = this.response.OutputStream;
            output.Write(buffer, 0, buffer.Length);
            // You must close the output stream.
            output.Close();
            // listener.Stop();
        }

        public void Send(int statusCode, Object payload)
        {
            // try serializing object into JSON
            // this will throw an exception if the payload is not a serializable object - i.e., has DataContractAttribute
            string serialized = JsonConvert.SerializeObject(payload);

            // Console.WriteLine("The Server Sends...");
            // Console.WriteLine(serialized);

            // Construct a response.
            // string responseString = "<HTML><BODY> Hello world!</BODY></HTML>";
            byte[] buffer = System.Text.Encoding.UTF8.GetBytes(serialized);

            // Get a response stream and write the response to it.
            this.response.ContentLength64 = buffer.Length;
            this.response.StatusCode = statusCode;
            this.response.ContentEncoding = Encoding.UTF8;
            this.response.ContentType = "application/json; charset=utf-8";
            System.IO.Stream output = this.response.OutputStream;
            output.Write(buffer, 0, buffer.Length);
            // You must close the output stream.
            output.Close();
            // listener.Stop();
        }
    }

    // This is an HTTP Server providing an asynchronous API like Express.js
    // It should run strictly on a single thread, otherwise things like
    // session data will be under race condition.
    class HttpServer : Router
    {
        private string host;
        private int port;
        private HttpListener listener;

        public HttpServer(string host, int port) : base()
        {
            this.host = host;
            this.port = port;

            this.listener = new HttpListener();
            this.listener.Prefixes.Add("http://" + host + ":" + port.ToString() + "/");
        }

        // Listen returns immediately, and the "listening" is done asynchronously via Task loop.
        public void Listen()
        {
            listener.Start();
            Console.WriteLine("Listening...");

            Helpers.AsyncLoop(HandleRequest);
        }

        // top-level request handler, should wrap low-level API
        // and call subsequent handlers
        private void HandleRequest()
        {
            HttpListenerContext context = listener.GetContext();
            
            // First check if this is a websocket handshake
            if (!context.Request.IsWebSocketRequest)
            {
                /*
                Console.WriteLine("WebSocket Request Received!!!");
                context.AcceptWebSocketAsync(null)
                    .ContinueWith(prev => this.wss.AddClient(prev.Result));
            }
            else
            {
                */
                Request request = new Request(context.Request);
                Response response = new Response(context.Response);

                // Before passing the request on to user-defined middleware functions,
                // route the request depending on the request method and path
                base.HandleRequest(request.pathSegments.Skip(1).ToArray(), request, response);
            }
        }
    }
}