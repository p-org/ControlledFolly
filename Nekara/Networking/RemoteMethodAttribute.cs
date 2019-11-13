using System;
using System.Collections.Generic;
using System.Text;

namespace Nekara.Networking
{
    public class RemoteMethodAttribute : Attribute
    {
        public string name;
        public string description;
    }
}
