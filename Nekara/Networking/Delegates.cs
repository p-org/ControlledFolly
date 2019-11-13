using System.Threading.Tasks;
using Newtonsoft.Json.Linq;

namespace Nekara.Networking
{
    public delegate Task<JToken> RemoteMethodAsync(string senderId, params JToken[] args);
}
