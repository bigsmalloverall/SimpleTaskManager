#include "SimpleTaskManager.h"

namespace SimpleTM
{
    SimpleTaskManager::SimpleTaskManager(TasksPool *tasksPool)
    {
        _tasksPool = tasksPool;
        _runningTasks = new LinkedList<Task *>();
    }

    SimpleTaskManager::SimpleTaskManager()
    {
        _runningTasks = new LinkedList<Task *>();
    }

    SimpleTaskManager::~SimpleTaskManager()
    {
        if (_runningTasks != nullptr)
        {
            delete _runningTasks;
        }
    }

    Task *SimpleTaskManager::fetch(uint16_t id, bool wrapId)
    {
        if (wrapId)
        {
            if (id > _tasksPool->count())
            {
                id = 0;
            }
            else if (id < 0)
            {
                id = _tasksPool->count() - 1;
            }
        }

#ifdef ARDUINO_TASKER_DEBUG
        Serial.print("SimpleTaskManager fetched task with id: ");
        Serial.println(id);
#endif

        return _tasksPool->tasks(id);
    }

    bool SimpleTaskManager::startTask(uint16_t id, bool wrapId)
    {
#ifdef ARDUINO_TASKER_DEBUG
        Serial.print("SimpleTaskManager Starting task with id: ");
        Serial.print(id);
        Serial.print("\t id wrapping: ");
        Serial.println(wrapId);
#endif

        Task *task = this->fetch(id, wrapId);
        return this->startTaskByPointer(task);
    }

    bool SimpleTaskManager::startTaskByPointer(Task *task)
    {

        if (task != nullptr)
        {
#ifdef ARDUINO_TASKER_DEBUG
            Serial.print("SimpleTaskManager Starting task:  ");
            Serial.print(task->getId());
            Serial.println("by pointer");
#endif
            _runningTasks->add(task);
            return true;
        }

        return false;
    }

    void SimpleTaskManager::startAllTasks()
    {
#ifdef ARDUINO_TASKER_DEBUG
        Serial.println("SimpleTaskManager Starting all tasks");
#endif
        if (_tasksPool != nullptr)
        {
            for (uint16_t i = 0; i < _tasksPool->count(); i++)
            {
                this->startTaskByPointer(this->fetch(i, false));
            }
        }
    }

    bool SimpleTaskManager::stopTask(uint16_t id)
    {
#ifdef ARDUINO_TASKER_DEBUG
        Serial.print("SimpleTaskManager Stopping task: ");
        Serial.println(id);
#endif
        Task *task = nullptr;

        for (uint16_t i = 0; i < (uint16_t)_runningTasks->size(); i++)
        {
            task = _runningTasks->get(i);

            if (task->canBeDeleted())
            {
                if (task->getId() == id)
                {
#ifdef ARDUINO_TASKER_DEBUG
                    Serial.print("SimpleTaskManager Task ");
                    Serial.print(task->getId());
                    Serial.println(" about to be deleted!");
#endif
                    _runningTasks->remove(i);
                    delete task;
                    return true;
                }
            }
            else
            {
#ifdef ARDUINO_TASKER_DEBUG
                Serial.print("SimpleTaskManager Task: ");
                Serial.print(id);
                Serial.println(" marked as \" Do not delete \"");
#endif
                return false;
            }
        }

        return false;
    }

    void SimpleTaskManager::stopAllTasks()
    {
#ifdef ARDUINO_TASKER_DEBUG
        Serial.print("SimpleTaskManager Stopping all tasks");
#endif
        Task *task = nullptr;

        for (uint16_t i = 0; i < (uint16_t)_runningTasks->size(); i++)
        {
            task = _runningTasks->get(i);

            if (task->canBeDeleted())
            {
                _runningTasks->remove(i);
            }
        }
    }

    uint32_t SimpleTaskManager::getDelta(Task *task)
    {
        return (uint32_t)micros() - task->getLastUpdateTime();
    }

    void SimpleTaskManager::update()
    {
        Task *task = nullptr;

        for (uint16_t i = 0; i < (uint16_t)_runningTasks->size(); i++)
        {
            task = _runningTasks->get(i);

            if (task != nullptr)
            {
                if (task->isDone())
                {
#ifdef ARDUINO_TASKER_DEBUG
                    Serial.print("SimpleTaskManager Task ");
                    Serial.print(task->getId());
                    Serial.println(" about to be deleted!");
#endif
                    this->stopTask(task->getId(), i);
                }
                else
                {
                    task->cache();
                    task->update(this->getDelta(task));
                    task->endUpdate();
                }
            }
        }
    }

    void SimpleTaskManager::setTasksPool(TasksPool *tasksPool)
    {
        _tasksPool = tasksPool;
    }

    bool SimpleTaskManager::isTaskDone(uint16_t id)
    {
        Task *task = this->getTask(id);
        return (task == nullptr ? true : task->isDone());
    }

    bool SimpleTaskManager::isTaskRunning(uint16_t id)
    {
        Task *task = this->getTask(id);
        return (task == nullptr ? false : !task->isDone());
    }

    bool SimpleTaskManager::doesTaskExist(uint16_t id)
    {
        Task *task = this->getTask(id);
        return task != nullptr;
    }

    Task *SimpleTaskManager::getTask(uint16_t id)
    {
        Task *task = nullptr;

        for (uint16_t i = 0; i < (uint16_t)_runningTasks->size(); i++)
        {
            task = _runningTasks->get(i);

            if (task->getId() == id)
            {
                return task;
            }
        }

        return nullptr;
    }

    void SimpleTaskManager::removeTaskFromRunningTasks(uint16_t id)
    {
        Task *task = nullptr;

        for (uint16_t i = 0; i < (uint16_t)_runningTasks->size(); i++)
        {
            task = _runningTasks->get(i);

            if (task->getId() == id)
            {
                _runningTasks->remove(i);
                return;
            }
        }
    }

    bool SimpleTaskManager::stopTask(uint16_t id, uint16_t pos)
    {
        if (pos >= _runningTasks->size())
        {
            return false;
        }

        Task *task = _runningTasks->get(pos);

        if (task->getId() == id)
        {
            _runningTasks->remove(pos);
            delete task;
        }
    }

} // namespace SimpleTM
