using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Nekara.Networking
{
    public interface IClient : IDisposable
    {
        (Task<JToken>, CancellationTokenSource) SendRequest(string func);
        (Task<JToken>, CancellationTokenSource) SendRequest(string func, JArray args);
        (Task<JToken>, CancellationTokenSource) SendRequest(string func, params JToken[] args);
        (Task<JToken>, CancellationTokenSource) SendRequest(string func, params bool[] args);
        (Task<JToken>, CancellationTokenSource) SendRequest(string func, params int[] args);
        (Task<JToken>, CancellationTokenSource) SendRequest(string func, params string[] args);

        void AddRemoteMethod(string name, RemoteMethodAsync handler);
    }
}
