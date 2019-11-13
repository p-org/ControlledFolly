namespace Nekara.Core
{
    public interface ITestingService
    {
        void CreateTask();

        void StartTask(int taskId);

        void EndTask(int taskId);

        void CreateResource(int resourceId);

        void DeleteResource(int resourceId);

        void BlockedOnResource(int resourceId);

        void SignalUpdatedResource(int resourceId);

        bool CreateNondetBool();

        int CreateNondetInteger(int maxValue);

        void Assert(bool value, string message);

        void ContextSwitch();

        string WaitForMainTask();
    }
}
