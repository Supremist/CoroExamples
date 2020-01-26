#include <iostream>
#include <experimental/coroutine>
#include <stdexcept>

struct Promise {
	using CoroHandle = std::experimental::coroutine_handle<Promise>;
	CoroHandle get_return_object() { return CoroHandle::from_promise(*this); }
	auto initial_suspend() { return std::experimental::suspend_never(); }
	auto final_suspend() { return std::experimental::suspend_always(); }

	void return_value(int val)
	{
		msg = "Returned int";
		value = val;
	}

	void return_value(double val)
	{
		msg = "Returned double";
		value = int(val);
	}

//	void return_void()
//	{
//		msg = "Returned void";
//		value = 0;
//	}

	auto yield_value(int val)
	{
		value = val;
		return std::experimental::suspend_always();
	}

	void unhandled_exception()
	{
		ex = std::current_exception();
	}

	std::string getMessage()
	{
		try {
			if (ex) {
				std::rethrow_exception(ex);
			}
			return msg;
		} catch (const std::exception &e) {
			return std::string(e.what());
		}
	}

	std::string msg;
	int value = 0;
	std::exception_ptr ex;
};

namespace std::experimental {
template<>
struct coroutine_traits<Promise::CoroHandle, int> {
	using promise_type = Promise;
};
}


Promise::CoroHandle myFirstCoroutine(int yieldCount)
{
//	std::cout << "Hello" << std::endl;
//	co_await std::experimental::suspend_always();
//	std::cout << "Coroutine" << std::endl;
//	//throw std::logic_error("Life has no meaning");
//	co_await std::experimental::suspend_always();
	std::cout << "Calculating the meaning of life" << std::endl;
	for (auto i = 0; i < yieldCount; ++i) {
		co_yield i;
	}
	co_return 42.2;
}

int main(int argc, char *argv[])
{
	auto coro = myFirstCoroutine(5);
	while (!coro.done()) {
		coro.resume();
		std::cout << "Yield: " << coro.promise().value << std::endl;
	}
	if (!coro.promise().ex) {
		std::cout << "The meaning of life is: " << coro.promise().value << std::endl;
	}
	std::cout << "Additional msg: " << coro.promise().getMessage().c_str() << std::endl;
}
