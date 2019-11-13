using System;
using System.Linq;

namespace Nekara.Core
{
    public enum DecisionType { ContextSwitch, CreateNondetBool, CreateNondetInteger }

    public class DecisionTrace
    {
        public DecisionType decisionType;
        public int decisionValue;           // chosen task ID or generated random value
        public int currentTask;
        public (int, int)[] tasks;

        public DecisionTrace(DecisionType decisionType, int decisionValue, int currentTask, (int, int)[] tasks)
        {
            this.decisionType = decisionType;
            this.decisionValue = decisionValue;
            this.currentTask = currentTask;
            this.tasks = tasks;
        }

        public DecisionTrace(DecisionType decisionType, bool decisionValue, int currentTask, (int, int)[] tasks) : this(decisionType, decisionValue ? 1 : 0, currentTask, tasks) { }

        public string Type { get
            {
                switch (this.decisionType)
                {
                    case DecisionType.ContextSwitch: return "ContextSwitch";
                    case DecisionType.CreateNondetBool: return "CreateNondetBool";
                    case DecisionType.CreateNondetInteger: return "CreateNondetInteger";
                    default: throw new Exception("Unknown Decision Type");
                }
            } }

        public string Value { get
            {
                return this.decisionType == DecisionType.CreateNondetBool ? (this.decisionValue == 0 ? "False" : "True") : this.decisionValue.ToString();
            } }

        public override string ToString()
        {
            return decisionType + "," + decisionValue.ToString() + "," +  currentTask.ToString() + "," + String.Join(";", tasks.Select(tup => tup.Item1.ToString() + ":" + tup.Item2.ToString()));
        }

        public string ToReadableString()
        {
            return this.Type + " -> " + this.Value + ", Current Task: " + currentTask.ToString() + ", All Tasks: [" + String.Join(", ", tasks.Select(tup => tup.Item1.ToString() + (tup.Item2 > -1 ? " |" + tup.Item2.ToString() : ""))) + "]";
        }

        public static DecisionTrace FromString(string line)
        {
            var cols = line.Split(',');
            DecisionType decisionType = (DecisionType)Enum.Parse(typeof(DecisionType), cols[0]);
            int decisionValue = Int32.Parse(cols[1]);
            int currentTask = Int32.Parse(cols[2]);
            (int, int)[] tasks = cols[3].Split(';').Select(t => t.Split(':')).Select(t => (Int32.Parse(t[0]), Int32.Parse(t[1]))).ToArray();
            return new DecisionTrace(decisionType, decisionValue, currentTask, tasks);
        }

        public override bool Equals(object obj)
        {
            if (obj is DecisionTrace)
            {
                var other = (DecisionTrace)obj;
                var myTasks = tasks.OrderBy(tup => tup.Item1).ToArray();
                var otherTasks = other.tasks.OrderBy(tup => tup.Item1).ToArray();

                bool match = (myTasks.Count() == otherTasks.Count())
                    && decisionType == other.decisionType
                    && decisionValue == other.decisionValue
                    && currentTask == other.currentTask
                    && myTasks.Select((tup, i) => otherTasks[i].Item1 == tup.Item1 && otherTasks[i].Item2 == tup.Item2).Aggregate(true, (acc, b) => acc && b);

                return match;
            }
            return false;
        }

        public static bool operator ==(DecisionTrace t1, DecisionTrace t2)
        {
            return t1.Equals(t2);
        }

        public static bool operator !=(DecisionTrace t1, DecisionTrace t2)
        {
            return !t1.Equals(t2);
        }
    }
}
