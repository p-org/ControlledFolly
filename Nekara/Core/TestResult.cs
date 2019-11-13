using System;

namespace Nekara.Core
{
    public struct TestResult
    {
        public int seed;
        public bool passed;
        public string reason;
        public double elapsedMs;
        
        public TestResult(int seed, bool passed, string reason = "", double elapsedMs = -1)
        {
            this.seed = seed;
            this.passed = passed;
            this.reason = reason;
            this.elapsedMs = elapsedMs;
        }

        override public string ToString()
        {
            return this.reason;
        }

        public string Serialize()
        {
            return this.seed.ToString() + "," + this.passed.ToString() + "," + this.reason + "," + this.elapsedMs.ToString();
        }

        public static TestResult Deserialize(string data)
        {
            string[] args = data.Split(',');
            return new TestResult(Int32.Parse(args[0]), Boolean.Parse(args[1]), args[2], Double.Parse(args[3]));
        }
    }
}
