using System.Runtime.Serialization;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Nekara.Networking
{
    // This Message construct is 1 layer above the communication layer
    // This extra level of abstraction is useful for remaining protocol-agnostic,
    // so that we can plug-in application layer transport protocols
    [DataContract]
    public class RequestMessage
    {
        [DataMember]
        internal string id;

        [DataMember]
        internal string sender;

        [DataMember]
        internal string recipient;

        [DataMember]
        internal string func;

        [DataMember]
        internal JToken[] args;

        public RequestMessage(string sender, string recipient, string func, JToken[] args)
        {
            this.id = "req-" + Helpers.RandomString(16);
            this.sender = sender;
            this.recipient = recipient;
            this.func = func;
            this.args = args;
        }

        public string Serialize()
        {
            return JsonConvert.SerializeObject(this);
        }

        public static RequestMessage Deserialize(string payload)
        {
            return JsonConvert.DeserializeObject<RequestMessage>(payload);
        }

        public ResponseMessage CreateResponse(string sender, JToken data)
        {
            return new ResponseMessage(sender, this.sender, this.id, data);
        }

        public ResponseMessage CreateErrorResponse(string sender, JToken data)
        {
            return new ResponseMessage(sender, this.sender, this.id, data, true);
        }
    }
}
