using System;
using System.Threading;
using System.Threading.Tasks;

namespace Nekara
{
    public class AsyncLock
    {
        public class Releaser : IDisposable
        {
            private TaskCompletionSource<bool> tcs;
            private string label;

            public Task<bool> Task
            {
                get { return this.tcs.Task; }
            }

            public Releaser(AsyncLock holder, string caller = "Creator")
            {
                this.tcs = new TaskCompletionSource<bool>();
                this.label = holder.label;
                Interlocked.Exchange(ref holder.releaser, this);
                // Console.WriteLine("  ... Thread {1} acquired lock {0}!", this.label, Thread.CurrentThread.ManagedThreadId);
            }

            public void Dispose(string caller = "")
            {
                this.tcs.SetResult(true);
                // Console.WriteLine("  ... Thread {1} released {0}", this.label, Thread.CurrentThread.ManagedThreadId);
            }

            public void Dispose()
            {
                this.Dispose("Unknown");
            }
        }

        private string label;
        private Releaser releaser;
        private readonly object locker;

        public AsyncLock(string label)
        {
            this.label = label;
            this.releaser = null;
            this.locker = new object();
        }

        public Task<Releaser> Acquire(string caller = "")
        {
            lock (this.locker) // we need this lock because there is a race condition for the this.releaser reference
            {
                // It is important that we use the default Task scheduler
                // to make this lock work correctly with arbitrary frameworks.
                // For instance, without specifying the default task scheduler, this lock fails in orleans,
                // as described in: http://dotnet.github.io/orleans/Documentation/grains/external_tasks_and_grains.html
                // The orleans task scheduler will enforce a single-threaded execution model and the lock will deadlock against the async IO operation

                if (this.releaser != null) return this.releaser.Task.ContinueWith(prev => CreateReleaser(), TaskScheduler.Default);
                else return Task.FromResult(CreateReleaser());
            }
        }

        private Releaser CreateReleaser()
        {
            return new Releaser(this);
        }

        public static AsyncLock Create(string label)
        {
            return new AsyncLock(label);
        }

        public static AsyncLock Create()
        {
            return new AsyncLock("Anonymous");
        }
    }
}
