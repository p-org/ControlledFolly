using Newtonsoft.Json.Linq;
using System;
using System.Reflection;
using System.Runtime.Serialization;

namespace Nekara
{
    public static class Exceptions
    {
        public static Exception DeserializeServerSideException(JToken payload)
        {
            // we assume that the payload is the serialized Exception object, and cast it to JObject
            // WARNING: if the server sends anything other than JObject,
            // this will throw an exception silently and will be swallowed!
            Console.WriteLine("\n  !!!----- Server Threw an Exception -----!!!");
            var serialized = payload.ToObject<JObject>();
            Console.WriteLine("\n  |  ClassName: {0}\n  |  Message: {1}\n  |  StackTrace:\n  |  {2}", serialized["ClassName"], serialized["Message"], serialized["StackTraceString"]);
            Console.WriteLine("  -------------------------------------------\n");

            var ExceptionType = Assembly.GetExecutingAssembly().GetType(serialized["ClassName"].ToObject<string>());
            var exception = (Exception)Activator.CreateInstance(ExceptionType, new[] { serialized["Message"].ToObject<string>() });

            return exception;
        }
    }

    public abstract class NekaraException : Exception
    {
        public NekaraException(string message) : base(message) { }
        public NekaraException(string message, Exception inner) : base(message, inner) { }
    }

    /* Test related */
    public abstract class TestingServiceException : NekaraException, ISerializable
    {
        public TestingServiceException(string message) : base(message) { }
        public TestingServiceException(string message, Exception inner) : base(message, inner) { }

        override public void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("ClassName", this.GetType().FullName, typeof(string));
            info.AddValue("Message", this.Message, typeof(string));
        }
    }

    [Serializable]
    public class AssertionFailureException : TestingServiceException
    {
        public AssertionFailureException(string message) : base(message)
        {
        }
    }

    [Serializable]
    public class TraceReproductionFailureException : TestingServiceException
    {
        public TraceReproductionFailureException(string message) : base(message)
        {
        }
    }

    [Serializable]
    public class TestFailedException : TestingServiceException
    {
        public TestFailedException(string message) : base(message)
        {
        }
    }

    // Exception thrown by child tasks, which are used to short-circuit the control flow - i.e. to avoid resuming the continuation
    // These should be ignored by the parent task.
    public class IntentionallyIgnoredException : TestingServiceException
    {
        public IntentionallyIgnoredException(string message, Exception inner) : base(message, inner)
        {
        }
    }

    /* Config related */
    class TesterConfigurationException : NekaraException
    {
        public TesterConfigurationException(string message) : base(message) { }
    }
    public class TestMethodLoadFailureException : NekaraException {
        public TestMethodLoadFailureException(string message) : base(message) { }
    }

    /* Communication related */
    // this is the client-side representation of the exception thrown by the server
    /*public class ServerThrownException : Exception
    {
        private JObject serialized;

        public ServerThrownException(JToken payload)
        {
            // we assume that the payload is the serialized Exception object, and cast it to JObject
            // WARNING: if the server sends anything other than JObject,
            // this will throw an exception silently and will be swallowed!
            this.serialized = payload.ToObject<JObject>();
        }

        public string ClassName { get { return this.serialized["ClassName"].ToObject<string>(); } }

        public override string Message { get { return this.serialized["Message"].ToObject<string>(); } }
    }*/

    public abstract class LogisticalException : NekaraException, ISerializable
    {
        public LogisticalException (string message) : base (message) { }

        override public void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("ClassName", this.GetType().FullName, typeof(string));
            info.AddValue("Message", this.Message, typeof(string));
        }
    }

    [Serializable]
    public class InvalidRequestPayloadException : LogisticalException
    {
        public InvalidRequestPayloadException(string message) : base(message) { }
    }

    [Serializable]
    public class RemoteMethodDoesNotExistException : LogisticalException
    {
        public RemoteMethodDoesNotExistException(string message) : base(message) { }
    }

    [Serializable]
    public class SessionRecordNotFoundException : LogisticalException
    {
        public SessionRecordNotFoundException(string message) : base(message) { }
    }

    [Serializable]
    public class SessionAlreadyFinishedException : LogisticalException
    {
        public SessionAlreadyFinishedException(string message) : base(message) { }
    }

    public class TestMethodNotFoundException : NekaraException
    {
        public TestMethodNotFoundException(string message) : base(message) { }
    }
    public class RequestTimeoutException : NekaraException {
        public RequestTimeoutException(string message) : base(message) { }
    }
    public class UnexpectedMessageException : NekaraException {
        public UnexpectedMessageException(string message) : base(message) { }
    }
    public class UnexpectedResponseException : NekaraException {
        public UnexpectedResponseException(string message) : base(message) { }
    }
    public class UnexpectedRequestException : NekaraException {
        public UnexpectedRequestException(string message) : base(message) { }
    }
}
