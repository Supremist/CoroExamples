#pragma once

#include <experimental/coroutine>
#include <stdexcept>

template<typename T>
class Generator
{
public:
	class promise_type;
	using CoroHandle = std::experimental::coroutine_handle<promise_type>;

	Generator() = default;
	Generator(Generator &&other);
	Generator(const Generator &) = delete;
	~Generator();

	T nextValue();
	bool hasNextValue() const;

private:
	Generator(const CoroHandle &handle);

	CoroHandle m_handle;
};

template<typename T>
class Generator<T>::promise_type
{
public:
	Generator<T> get_return_object() { return Generator<T>::CoroHandle::from_promise(*this); }

	auto initial_suspend() { return std::experimental::suspend_always(); }
	auto final_suspend() { return std::experimental::suspend_always(); }

	auto yield_value(T val) { m_currentValue = val; return std::experimental::suspend_always(); }
	//void return_value(T val) { m_currentValue = val;}

	void unhandled_exception() { m_exception = std::current_exception(); }

	T getValue();

private:
	T m_currentValue;
	std::exception_ptr m_exception;
};

template<typename T>
Generator<T>::Generator(const Generator<T>::CoroHandle &handle)
    : m_handle(handle)
{
}

template<typename T>
Generator<T>::Generator(Generator &&other)
{
	std::swap(this->m_handle, other.m_handle);
}

template<typename T>
Generator<T>::~Generator()
{
	if (m_handle) {
		m_handle.destroy();
	}
}

template<typename T>
bool Generator<T>::hasNextValue() const
{
	return m_handle && !m_handle.done();
}

template<typename T>
T Generator<T>::nextValue()
{
	m_handle.resume();
	return m_handle.promise().getValue();
}

template<typename T>
T Generator<T>::promise_type::getValue()
{
	if (m_exception) {
		std::rethrow_exception(m_exception);
	}
	return m_currentValue;
}
