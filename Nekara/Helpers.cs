using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace Nekara
{
    public class Helpers
    {
        private static Random random = new Random(DateTime.Now.Millisecond);
        public static string RandomString(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            return new string(Enumerable.Repeat(chars, length)
              .Select(s => s[random.Next(s.Length)]).ToArray());
        }

        public static int RandomInt(int maxValue = 1000)
        {
            return random.Next(maxValue);
        }

        public static bool RandomBool()
        {
            bool result = false;
            if (random.Next(2) == 0)
            {
                result = true;
            }
            return result;
        }

        public class SeededRandomizer
        {
            private Random random;
            public SeededRandomizer(int seed = 0)
            {
                this.random = new Random(seed);
            }

            public string NextString(int length)
            {
                const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
                return new string(Enumerable.Repeat(chars, length)
                  .Select(s => s[this.random.Next(s.Length)]).ToArray());
            }

            public int NextInt(int maxValue)
            {
                return this.random.Next(maxValue);
            }

            public bool NextBool()
            {
                bool result = false;
                if (this.random.Next(2) == 0)
                {
                    result = true;
                }
                return result;
            }
        }

        public class UniqueIdGenerator
        {
            private SeededRandomizer randomizer = new SeededRandomizer(DateTime.Now.Second);
            private HashSet<int> issued = new HashSet<int>();
            private bool sequentialMode;
            private int idOffset;

            public UniqueIdGenerator(bool sequential = true, int idOffset = 1000001)
            {
                sequentialMode = sequential;
                this.idOffset = idOffset;
            }

            public int Generate()
            {
                lock (issued)
                {
                    int x;
                    do
                    {
                        x = sequentialMode ? idOffset + issued.Count : randomizer.NextInt(Int32.MaxValue);
                    } while (issued.Contains(x) || x < 1);
                    issued.Add(x);
                    return x;
                }
            }

            public void Reset()
            {
                lock (issued)
                {
                    issued.Clear();
                }
            }
        }

        public class MicroProfiler
        {
            private Dictionary<(string, string), (int, double)> Data = new Dictionary<(string, string), (int, double)>();
            // private string LastPoint = null;
            // private DateTime LastStamp;

            public MicroProfiler() { }

            public (string, DateTime) Update(string point)
            {
                return (point, DateTime.Now);
            }
            
            public (string, DateTime) Update(string point, (string, DateTime) lastDatum)
            {
                var Now = DateTime.Now;
                if (!Data.ContainsKey((lastDatum.Item1, point)))
                {
                    Data[(lastDatum.Item1, point)] = (0, 0.0);
                }

                lock (Data)
                {
                    var (count, average) = Data[(lastDatum.Item1, point)];
                    Data[(lastDatum.Item1, point)] = (count + 1, ((Now - lastDatum.Item2).Ticks + count * average) / (count + 1));
                }
                return (point, Now);
            }

            public override string ToString()
            {
                return string.Join("\n", Data.Select(item => "[ " + item.Key.Item1 + " ~ " + item.Key.Item2 + " ] " + item.Value.Item2 + " ticks (" + item.Value.Item1 + " times)"));
            }
        }

        public static int PromptInt(string prompt, int min = 0, int max = 100)
        {
            Console.Write(prompt);
            int input = Int32.Parse(Console.ReadLine());
            while (input < min || max < input)
            {
                Console.WriteLine("Invalid Value, enter a value between {0} and {1}\n", min, max);
                Console.Write(prompt);
                input = Int32.Parse(Console.ReadLine());
            }
            return input;
        }

        public static string Prompt(string prompt, Func<string, bool> verifier, bool collapseWhitespace = true)
        {
            Console.Write(prompt);
            string input = Console.ReadLine();
            if (collapseWhitespace) input = Regex.Replace(input, @"[ \t]+", " ");
            while (!verifier(input))
            {
                Console.WriteLine("Invalid Value\n");
                Console.Write(prompt);
                input = Console.ReadLine();
            }
            return input;
        }

        public static void AsyncLoop(Action action)
        {
            // TODO: Need to handle exceptions - either provide a way to handle it
            //       or throw the error to the main thread.
            //       Any exception thrown here will be swallowed silently!!!
            Task.Run(action).ContinueWith(prev => AsyncLoop(action));   // Will this lead to memory leak?
        }

        public static void AsyncTaskLoop(Func<Task> action)
        {
            // TODO: Need to handle exceptions - either provide a way to handle it
            //       or throw the error to the main thread.
            //       Any exception thrown here will be swallowed silently!!!
            action().ContinueWith(prev => AsyncTaskLoop(action));   // Will this lead to memory leak?
        }

        public static void AsyncTaskLoop(Func<Task> action, CancellationToken token)
        {
            // TODO: Need to handle exceptions - either provide a way to handle it
            //       or throw the error to the main thread.
            //       Any exception thrown here will be swallowed silently!!!
            if (token.IsCancellationRequested)
            {
                Console.WriteLine("... Cancelled");
                return;
            }
            action().ContinueWith(prev => AsyncTaskLoop(action, token));   // Will this lead to memory leak?
        }

        public static Task RepeatTask(Func<Task> action, int count)
        {
            if (count > 1) return action().ContinueWith(prev => RepeatTask(action, count - 1)).Unwrap();
            return action();
        }

        public class TaskLock
        {

        }
    }
}