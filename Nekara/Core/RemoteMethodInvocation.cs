using System;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;

namespace Nekara.Core
{
    public class RemoteMethodInvocation
    {
        // parameters
        public readonly object Instance;
        public readonly MethodInfo Function;
        public readonly object[] Arguments;

        // run-time data
        public dynamic Result;
        public event EventHandler OnBeforeInvoke;
        public event EventHandler OnSuccess;
        public event EventHandler<Exception> OnError;
        public event EventHandler OnAfterInvoke;

        public RemoteMethodInvocation(object instance, MethodInfo func, object[] args)
        {
            Instance = instance;
            Function = func;
            Arguments = args;

            Result = null;
        }

        public override string ToString()
        {
            return this.Function.Name + "(" + String.Join(", ", Arguments.Select(arg => arg.ToString())) + ")";
        }

        public object Invoke()
        {
            OnBeforeInvoke(this, null);

            try
            {
                if (Function.ReturnType == typeof(void)) Result = Function.Invoke(Instance, Arguments);
                else Result = Function.Invoke(Instance, Arguments);

                return Result;
            }
            catch (TargetInvocationException ex)
            {
                // Console.WriteLine(ex);
                Console.WriteLine("\n[RemoteMethodInvocation: {0}] TargetInvocation/{1}", this.ToString(), ex.InnerException.GetType().Name);
                Exception inner;
                if (ex.InnerException is AssertionFailureException)
                {
                    inner = ex.InnerException;
                }
                else if (ex.InnerException is AggregateException)
                {
                    Console.WriteLine("\t  [RemoteMethodInvocation: {0}] TargetInvocation/Aggregate/{1}", this.ToString(), ex.InnerException.InnerException.GetType().Name);
                    inner = ex.InnerException.InnerException;
                }
                else if (ex.InnerException is TargetInvocationException)
                {
                    Console.WriteLine("\t  [RemoteMethodInvocation: {0}] TargetInvocation/TargetInvocation/{1}", this.ToString(), ex.InnerException.InnerException.GetType().Name);
                    inner = ex.InnerException.InnerException;
                }
                else {
                    Console.WriteLine(ex);
                    inner = ex.InnerException;
                }
                OnError(this, inner);
                throw inner;
            }
            catch (Exception ex)
            {
                Console.WriteLine("\n[RemoteMethodInvocation] Unexpected {0}", ex.GetType().Name);
                Console.WriteLine(ex);
                throw ex;
            }
            finally
            {
                OnAfterInvoke(this, null);
            }
        }
    }
}
