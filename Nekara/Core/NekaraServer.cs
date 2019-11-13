using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using Newtonsoft.Json.Linq;
using Nekara.Networking;

namespace Nekara.Core
{
    /* The objects below are transport-agnostic and deals only with the user-facing testing API.
     * The only thing related to the transport mechanism is the RemoteMethodAttribute
     */

    // This is the service object exposed to the client, and hosted on the server-side
    // The testing service API should be exposed by this object.
    public class NekaraServer : MarshalByRefObject
    {
        public static int gCount = 0;
        private Dictionary<string, TestingSession> testSessions;
        private StreamWriter summaryFile;

        public NekaraServer()
        {
            this.testSessions = new Dictionary<string, TestingSession>();

            this.summaryFile = File.AppendText("logs/summary-" + DateTime.Now.Ticks.ToString() + ".csv");
            this.summaryFile.WriteLine("Assembly,Class,Method,SessionId,Seed,Result,Reason,Elapsed");
        }

        [RemoteMethod(name = "InitializeTestSession", description = "Initializes server-side proxy program that will represent the actual program on the client-side")]
        // treating this method as a special case because it spawns another Task we have to resolve later
        public string InitializeTestSession(JObject arg)
        {
            var sessionInfo = SessionInfo.FromJson(arg);
            var session = new TestingSession(sessionInfo);

            session.OnComplete(record =>
            {
                Console.WriteLine("\n==========[ Test {0} {1} ]==========\n", record.sessionId, record.passed ? "PASSED" : "FAILED");
                if (record.reason != "")
                {
                    Console.WriteLine("  " + record.reason);
                }

                // Append Summary
                string summary = String.Join(",", new string[] { 
                    sessionInfo.assemblyName, 
                    sessionInfo.methodDeclaringClass, 
                    sessionInfo.methodName, 
                    session.Id,
                    sessionInfo.schedulingSeed.ToString(),
                    (record.passed ? "pass" : "fail"),
                    record.reason,
                    record.elapsedMs.ToString() });
                this.summaryFile.WriteLine(summary);
                this.summaryFile.Flush();

                Console.WriteLine("\n--------------------------------------------\n");
                Console.WriteLine("    Total Requests:\t{0}", record.numRequests);
                Console.WriteLine("    Avg Invoke Time:\t{0} ms", record.avgInvokeTime);
                Console.WriteLine("    Total Time Taken:\t{0} ms", record.elapsedMs);
                Console.WriteLine("\n===== END of {0} =====[ Results: {1}/{2} ]=====\n\n", session.Id, this.testSessions.Where(item => item.Value.LastRecord.passed == true).Count(), this.testSessions.Count);

            });

            lock (this.testSessions)
            {
                this.testSessions.Add(session.Id, session);
            }

            Console.WriteLine("\n\n===== BEGIN {0} ================================\n", session.Id);
            Console.WriteLine("  [{1}]\n  in {0}", sessionInfo.assemblyName, sessionInfo.methodDeclaringClass + "." + sessionInfo.methodName);
            Console.WriteLine("  Seed:\t\t{0}\n  Timeout:\t{1}\n  MaxDecisions:\t{2}\n", sessionInfo.schedulingSeed, sessionInfo.timeoutMs, sessionInfo.maxDecisions);
            //Console.WriteLine("\nIndex\tThrd\t#Thrds\tCurTask\t#Tasks\t#Blckd\tPending\tStage\tMethod\tArgs");

            return session.Id;
        }

        [RemoteMethod(name = "ReplayTestSession", description = "Replays the test session identified by the given session ID")]
        public SessionInfo ReplayTestSession(JToken arg)
        {
            string sessionId = arg.ToObject<string>();

            SessionInfo info = GetSessionInfo(sessionId);
            lock (this.testSessions)
            {
                TestingSession session = this.testSessions[sessionId];
                session.Reset();

                Console.WriteLine("\n\n===== BEGIN REPLAY {0} =========================\n", sessionId);
                Console.WriteLine("  [{1}]\n  in {0}\n", info.assemblyName, info.methodDeclaringClass + "." + info.methodName);
                Console.WriteLine("  Seed:\t\t{0}\n  Timeout:\t{1}\n  MaxDecisions:\t{2}\n", info.schedulingSeed, info.timeoutMs, info.maxDecisions);
            }

            return info;
        }

        [RemoteMethod(name = "GetSessionInfo", description = "Gets the Session info based on the session ID")]
        public SessionInfo GetSessionInfo(JToken arg)
        {
            string sessionId = arg.ToObject<string>();
            lock (this.testSessions)
            {
                if (!this.testSessions.ContainsKey(sessionId)) throw new SessionRecordNotFoundException("Session " + sessionId + " not found");
                return this.testSessions[sessionId].Meta;
            }
        }

        private object RouteRemoteCall(string sessionId, string methodName, params object[] args)
        {
            return this.testSessions[sessionId].InvokeAndHandleException(methodName, args);
        }

        /* Methods below are the actual methods called (remotely) by the client-side proxy object.
         * Some methods have to be wrapped by an overloaded method because RemoteMethods are
         * expected to have a specific signature - i.e., all arguments are given as JTokens
         */
        [RemoteMethod(name = "CreateTask", description = "Creates a new task")]
        public void CreateTask(JToken sessionId)
        {
            RouteRemoteCall(sessionId.ToObject<string>(), "CreateTask");
        }

        [RemoteMethod(name = "StartTask", description = "Signals the start of a given task")]
        public void StartTask(JToken sessionId, JToken taskId)
        {
            RouteRemoteCall(sessionId.ToObject<string>(), "StartTask", taskId.ToObject<int>());
        }

        [RemoteMethod(name = "EndTask", description = "Signals the end of a given task")]
        public void EndTask(JToken sessionId, JToken taskId)
        {
            RouteRemoteCall(sessionId.ToObject<string>(), "EndTask", taskId.ToObject<int>());
        }

        [RemoteMethod(name = "CreateResource", description = "Notifies the creation of a new resource")]
        public void CreateResource(JToken sessionId, JToken resourceId)
        {
            RouteRemoteCall(sessionId.ToObject<string>(), "CreateResource", resourceId.ToObject<int>());
        }

        [RemoteMethod(name = "DeleteResource", description = "Signals the deletion of a given resource")]
        public void DeleteResource(JToken sessionId, JToken resourceId)
        {
            RouteRemoteCall(sessionId.ToObject<string>(), "DeleteResource", resourceId.ToObject<int>());
        }

        [RemoteMethod(name = "BlockedOnResource", description = "")]
        public void BlockedOnResource(JToken sessionId, JToken resourceId)
        {
            RouteRemoteCall(sessionId.ToObject<string>(), "BlockedOnResource", resourceId.ToObject<int>());
        }

        [RemoteMethod(name = "SignalUpdatedResource", description = "")]
        public void SignalUpdatedResource(JToken sessionId, JToken resourceId)
        {
            RouteRemoteCall(sessionId.ToObject<string>(), "SignalUpdatedResource", resourceId.ToObject<int>());
        }

        [RemoteMethod(name = "CreateNondetBool", description = "")]
        public bool CreateNondetBool(JToken sessionId)
        {
            return (bool)RouteRemoteCall(sessionId.ToObject<string>(), "CreateNondetBool");
        }

        [RemoteMethod(name = "CreateNondetInteger", description = "")]
        public int CreateNondetInteger(JToken sessionId, JToken maxValue)
        {
            return (int)RouteRemoteCall(sessionId.ToObject<string>(), "CreateNondetInteger", maxValue.ToObject<int>());
        }

        [RemoteMethod(name = "Assert", description = "")]
        public void Assert(JToken sessionId, JToken value, JToken message)
        {
            RouteRemoteCall(sessionId.ToObject<string>(), "Assert", value.ToObject<bool>(), message.ToObject<string>());
        }

        [RemoteMethod(name = "ContextSwitch", description = "Signals the deletion of a given resource")]
        public void ContextSwitch(JToken sessionId)
        {
            RouteRemoteCall(sessionId.ToObject<string>(), "ContextSwitch");
        }

        [RemoteMethod(name = "WaitForMainTask", description = "Wait for test to finish")]
        public string WaitForMainTask(JToken sessionId)
        {
            return (string)RouteRemoteCall(sessionId.ToObject<string>(), "WaitForMainTask");
        }
    }
}