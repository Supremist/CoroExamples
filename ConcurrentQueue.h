#pragma once

#include "GuardedPointer.h"
#include <optional>

#include <list>
#include <mutex>
#include <condition_variable>

template<typename T>
class ConcurrentQueue
{
private:
	mutable std::mutex m_stateMutex;
	std::condition_variable m_cv;
	std::list<T> m_queue;

public:
	using UniqueLock = std::unique_lock<decltype(m_stateMutex)>;

	ConcurrentQueue() = default;

	template<typename U>
	void push(U &&item)
	{
		UniqueLock lk(m_stateMutex);
		m_queue.emplace_front(std::forward<U>(item));
		lk.unlock();
		m_cv.notify_one();
	}

	std::size_t size() const
	{
		UniqueLock lk(m_stateMutex);
		return m_queue.size();
	}

	bool empty() const
	{
		return size() == 0;
	}

	std::optional<T> tryPop()
	{
		UniqueLock lk(m_stateMutex, std::defer_lock);
		if (lk.try_lock()) {
			return popImpl();
		}
		return {};
	}

	std::optional<T> pop()
	{
		UniqueLock lk(m_stateMutex);
		return popImpl();
	}

	template<typename Pred>
	std::optional<T> popWait(Pred pred)
	{
		UniqueLock lk(m_stateMutex);
		m_cv.wait(lk, [this, pred] {
			return !m_queue.empty() || pred();
		});

		return popImpl();
	}

	std::optional<T> popWait()
	{
		return popWait([]{ return false; });
	}

	template<typename Func>
	void cancelWait(Func cancellation)
	{
		UniqueLock lk(m_stateMutex);
		cancellation();
		lk.unlock();
		m_cv.notify_all();
	}

protected:
	std::optional<T> popImpl()
	{
		if (m_queue.empty()) {
			return {};
		}

		T result = std::move(m_queue.back());
		m_queue.pop_back();
		return result;
	}

	ptr_guard::PointerGuard<decltype(m_stateMutex), std::list<T> *> lockQueue()
	{
		return ptr_guard::make_guarded_ptr(m_stateMutex, &m_queue);
	}
};
