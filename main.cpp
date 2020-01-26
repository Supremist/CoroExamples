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
	int value;
	std::exception_ptr ex;
};

namespace std::experimental {
template<>
struct coroutine_traits<Promise::CoroHandle> {
	using promise_type = Promise;
};
}


Promise::CoroHandle myFirstCoroutine()
{
	std::cout << "Hello" << std::endl;
	co_await std::experimental::suspend_always();
	std::cout << "Coroutine" << std::endl;
	throw std::logic_error("Life has no meaning");
	co_await std::experimental::suspend_always();
	std::cout << "Calculating the meaning of life" << std::endl;
	co_return 42.2;
}

int main(int argc, char *argv[])
{
	auto coro = myFirstCoroutine();
	while (!coro.done()) {
		std::cout << "Main: (doing useful work)" << std::endl;
		coro.resume();
	}
	if (!coro.promise().ex) {
		std::cout << "The meaning of life is: " << coro.promise().value << std::endl;
	}
	std::cout << "Additional msg: " << coro.promise().getMessage().c_str() << std::endl;
}
