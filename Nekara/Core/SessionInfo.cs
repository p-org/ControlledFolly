using System.Runtime.Serialization;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Nekara.Core
{
    [DataContract]
    public class SessionInfo
    {
        [DataMember]
        public string id;

        [DataMember]
        public string assemblyName;

        [DataMember]
        public string assemblyPath;

        [DataMember]
        public string methodDeclaringClass;

        [DataMember]
        public string methodName;

        [DataMember]
        public int schedulingSeed;

        [DataMember]
        public int timeoutMs;

        [DataMember]
        public int maxDecisions;

        public SessionInfo(string id, string assemblyName, string assemblyPath, string methodDeclaringClass, string methodName, int schedulingSeed, int timeoutMs = Constants.SessionTimeoutMs, int maxDecisions = Constants.SessionMaxDecisions)
        {
            this.id = id;
            this.assemblyName = assemblyName;
            this.assemblyPath = assemblyPath;
            this.methodDeclaringClass = methodDeclaringClass;
            this.methodName = methodName;
            this.schedulingSeed = schedulingSeed;
            this.timeoutMs = timeoutMs;
            this.maxDecisions = maxDecisions;
        }

        public string Serialize()
        {
            return JsonConvert.SerializeObject(this);
        }

        public JObject Jsonify()
        {
            return JObject.FromObject(this);
        }

        public static SessionInfo Deserialize(string payload)
        {
            return JsonConvert.DeserializeObject<SessionInfo>(payload);
        }

        public static SessionInfo FromJson(JObject data)
        {
            return new SessionInfo(data["id"].ToObject<string>(),
                data["assemblyName"].ToObject<string>(),
                data["assemblyPath"].ToObject<string>(),
                data["methodDeclaringClass"].ToObject<string>(),
                data["methodName"].ToObject<string>(),
                data["schedulingSeed"].ToObject<int>(),
                data["timeoutMs"].ToObject<int>(),
                data["maxDecisions"].ToObject<int>());
        }
    }
}
