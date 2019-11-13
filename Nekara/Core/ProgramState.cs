using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nekara.Core
{
    class ProgramState
    {
        // All tasks in the program and their TCS
        public Dictionary<int, TaskCompletionSource<bool>> taskToTcs;

        // Set of all resources in the system
        public HashSet<int> resourceSet;

        // task -> the resource its blocked on (if any)
        public Dictionary<int, int> blockedTasks;

        // current executing task
        public int currentTask;

        public ProgramState()
        {
            taskToTcs = new Dictionary<int, TaskCompletionSource<bool>>();
            resourceSet = new HashSet<int>();
            blockedTasks = new Dictionary<int, int>();

            currentTask = 0;
            taskToTcs.Add(0, new TaskCompletionSource<bool>());
            taskToTcs[0].SetResult(true);
        }
    }
}
