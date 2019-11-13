using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nekara
{
    // A class providing the same interface as JavaScript Promise
    public class Promise
    {
        private TaskCompletionSource<object> tcs;
        public Promise(Action<Action<object>, Action<object>> action)
        {
            this.tcs = new TaskCompletionSource<object>();
            Action<object> resolver = (object result) => {
                this.tcs.SetResult(result);
            };
            Action<object> rejector = (object error) => {
                this.tcs.SetException(new Exception());
            };
            action(resolver, rejector);
            // Console.WriteLine("Promise on Thread {0} / {1}", Thread.CurrentThread.ManagedThreadId, Process.GetCurrentProcess().Threads.Count);
        }

        public Task<object> Task { get { return this.tcs.Task; } }

        public Promise Then(Func<object, object> onResolve)
        {
            return new Promise((resolve, reject) =>
            {
                this.tcs.Task.ContinueWith(prev =>
                {
                    try
                    {
                        var nextResult = onResolve(prev.Result);
                        if (nextResult is Promise)
                        {
                            ((Promise)nextResult).Then(res =>
                            {
                                resolve(res);
                                return null;
                            }, err => {
                                Console.WriteLine(err);
                                reject(err);
                                return null;
                            });
                        }
                        else resolve(nextResult);
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                        reject(e);
                    }
                }, TaskContinuationOptions.OnlyOnRanToCompletion);

                this.tcs.Task.ContinueWith(prev => reject(prev.Exception), TaskContinuationOptions.OnlyOnFaulted);
            });
        }

        public Promise Then(Func<object, object> onResolve, Func<object, object> onReject)
        {
            return new Promise((resolve, reject) =>
            {
                this.tcs.Task.ContinueWith(prev =>
                {
                    try
                    {
                        resolve(onResolve(prev.Result));
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                        reject(e);
                    }
                }, TaskContinuationOptions.OnlyOnRanToCompletion);

                this.tcs.Task.ContinueWith(prev =>
                {
                    try
                    {
                        Console.WriteLine(prev.Exception);
                        resolve(onReject(prev.Exception));
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                        reject(e);
                    }
                }, TaskContinuationOptions.OnlyOnFaulted);
            });
        }

        public Promise Catch(Func<Exception, object> onReject)
        {
            return new Promise((resolve, reject) =>
            {
                this.tcs.Task.ContinueWith(prev => resolve(prev.Result), TaskContinuationOptions.OnlyOnRanToCompletion);

                this.tcs.Task.ContinueWith(prev =>
                {
                    try
                    {
                        Console.WriteLine(prev.Exception);
                        resolve(onReject(prev.Exception));
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                        reject(e);
                    }
                }, TaskContinuationOptions.OnlyOnFaulted);
            });
        }

        public Promise Finally(Func<object> onFinish)
        {
            return new Promise((resolve, reject) =>
            {
                this.tcs.Task.ContinueWith(prev =>
                {
                    try
                    {
                        resolve(onFinish());
                    }
                    catch (Exception e)
                    {
                        reject(e);
                    }
                });
            });
        }

        public static Promise Resolve(object value)
        {
            return new Promise((resolve, reject) => resolve(value));
        }

        public static Promise Reject(object error)
        {
            return new Promise((resolve, reject) => reject(error));
        }

        public static Promise All(IEnumerable<Promise> promises)
        {
            return Promise.Resolve(true);
        }

        public static Promise Race(IEnumerable<Promise> promises)
        {
            return Promise.Resolve(true);
        }

        public static Promise FromTask(Task task)
        {
            return new Promise((resolve, reject) =>
            {
                task.ContinueWith(prev => resolve(null), TaskContinuationOptions.OnlyOnRanToCompletion);
                task.ContinueWith(prev => reject(null), TaskContinuationOptions.OnlyOnFaulted);
            });
        }

        public static Promise FromTask(Task<object> task)
        {
            return new Promise((resolve, reject) =>
            {
                task.ContinueWith(prev => resolve(prev.Result), TaskContinuationOptions.OnlyOnRanToCompletion);
                task.ContinueWith(prev => reject(prev.Exception), TaskContinuationOptions.OnlyOnFaulted);
            });
        }
    }
}
