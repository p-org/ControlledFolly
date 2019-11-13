using System;
using System.Runtime.Serialization;
using System.Threading;
using Newtonsoft.Json;

namespace Nekara.Core
{
    [DataContract]
    public class SessionRecord
    {
        [DataMember]
        public string sessionId;

        [DataMember]
        public int schedulingSeed;

        [DataMember]
        public bool passed = false;

        [DataMember]
        public string reason;

        // performance data
        [DataMember]
        public DateTime startedAt;

        [DataMember]
        public DateTime finishedAt;

        [DataMember]
        public int numDecisions;

        [DataMember]
        public int numRequests;

        [DataMember]
        public double avgInvokeTime;

        public double elapsedMs { get { return (finishedAt - startedAt).TotalMilliseconds; } }

        public SessionRecord()
        {
        }
        
        public SessionRecord(string sessionId, int schedulingSeed)
        {
            this.sessionId = sessionId;
            this.schedulingSeed = schedulingSeed;
        }

        public override string ToString()
        {
            return "Session Record " + this.sessionId + " (" + (this.passed ? "Pass" : "Fail") + ")";
        }

        public void RecordBegin()
        {
            this.startedAt = DateTime.Now;
            this.numRequests = 0;
            this.avgInvokeTime = 0.0;
        }

        public void RecordEnd()
        {
            this.finishedAt = DateTime.Now;
        }

        public void RecordMethodCall(double elapsed)
        {
            Interlocked.Exchange(ref avgInvokeTime, (elapsed + numRequests * avgInvokeTime) / (numRequests + 1));
            Interlocked.Increment(ref numRequests);
        }

        public void RecordResult(bool passed, string reason)
        {
            this.passed = passed;
            this.reason = reason;
        }

        public string Serialize()
        {
            return JsonConvert.SerializeObject(this);
        }

        /*public JObject Jsonify()
        {
            return JObject.FromObject(this);
        }*/

        public static SessionRecord Deserialize(string payload)
        {
            return JsonConvert.DeserializeObject<SessionRecord>(payload);
        }

        /*public static SessionInfo FromJson(JObject data)
        {
            return new SessionInfo(data["id"].ToObject<string>(),
                data["assemblyName"].ToObject<string>(),
                data["assemblyPath"].ToObject<string>(),
                data["methodDeclaringClass"].ToObject<string>(),
                data["methodName"].ToObject<string>(),
                data["schedulingSeed"].ToObject<int>(),
                data["timeoutMs"].ToObject<int>(),
                data["maxDecisions"].ToObject<int>());
        }*/
    }
}
