#pragma once

#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <shared_mutex>

#define BREAK __debugbreak();

#define ALWAYS_ASSERT(expression, ...) \
	if (!(expression)) \
	{ \
		BREAK; \
		std::abort(); \
	}

#ifdef NDEBUG
#define DEV_ASSERT(...)
#else
#define DEV_ASSERT(expression, ...) ALWAYS_ASSERT(expression, __VA_ARGS__);
#endif


class ParallelExecutor
{
protected:
	void awake() { m_workCV.notify_all(); }

	void loop(uint32_t threadIndex);

	bool m_isLooping;

	std::atomic<uint32_t> m_finishedThreadNum;
	std::atomic<uint32_t> m_completedBatchNum;
	std::function<void(uint32_t)> m_executeTasks;

	std::shared_mutex m_mutex;
	std::condition_variable_any m_waitCV;
	std::condition_variable_any m_workCV;
	
	std::vector<std::thread> m_threads;

public:
	static uint32_t MAX_THREADS; // 100% CPU occupation, it may cause OS hitches.
								 // No point to have more threads than the number of CPU logical cores.

	static uint32_t HALF_THREADS; // 50-100% CPU occupation

	using Func = std::function<void(uint32_t, uint32_t)>; // (threadIndex, taskIndex)

	ParallelExecutor(uint32_t numThreads);
	~ParallelExecutor();

	uint32_t numThreads() const { return m_threads.size(); }
	bool isWorking() const { return m_finishedThreadNum < m_threads.size(); }

	void wait()
	{
		if (!isWorking()) return;

		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (!isWorking()) return; // re-check for a case when threads finished and m_waitCV is notified before the lock is acquired

		m_waitCV.wait(lock);
	}

	// Executes a function in parallel blocking the caller thread.
	void execute(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch)
	{
		if (numTasks == 0) return;
		executeAsync(func, numTasks, tasksPerBatch);
		wait();
	}

	// Executes a function in parallel asynchronously.
	void executeAsync(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch);

};

