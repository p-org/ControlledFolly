namespace Nekara.Networking
{
    public class OmniClientConfiguration
    {
        public string serviceHost;    // used if Transport == HTTP
        public int servicePort;       // used if Transport == HTTP or TCP

        public OmniClientConfiguration(Transport tMode = Transport.HTTP) : this(tMode, "localhost", 8080) { }

        public OmniClientConfiguration(Transport tMode, string serviceHost, int servicePort)
        {
            this.Transport = tMode;
            this.serviceHost = serviceHost;
            this.servicePort = servicePort;
        }

        public Transport Transport { get; set; }
    }
}
