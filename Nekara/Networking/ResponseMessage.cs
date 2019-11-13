using System.Runtime.Serialization;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Nekara.Networking
{
    // This Message construct is 1 layer above the communication layer
    // This extra level of abstraction is useful for remaining protocol-agnostic,
    // so that we can plug-in application layer transport protocols
    [DataContract]
    public class ResponseMessage
    {
        [DataMember]
        internal string id;

        [DataMember]
        internal string sender;

        [DataMember]
        internal string recipient;

        [DataMember]
        internal string responseTo;

        [DataMember]
        internal JToken data;

        [DataMember]
        internal bool error;

        public ResponseMessage(string sender, string recipient, string requestId, JToken data, bool isError = false)
        {
            this.id = "res-" + Helpers.RandomString(16);
            this.sender = sender;
            this.recipient = recipient;
            this.responseTo = requestId;
            this.data = data;
            this.error = isError;
        }

        public string Serialize()
        {
            return JsonConvert.SerializeObject(this);
        }

        public static ResponseMessage Deserialize(string payload)
        {
            return JsonConvert.DeserializeObject<ResponseMessage>(payload);
        }
    }
}
