#include <iostream>
#include <memory>

struct CoroutineFrame
{
	int state;
	int i = 0;
};

struct Resumable
{
	Resumable();

	bool resume();

	std::shared_ptr<CoroutineFrame> c;
};

Resumable::Resumable()
    : c(std::make_shared<CoroutineFrame>())
{
	c->state = 0;
}

bool Resumable::resume()
{
#define co_yield(val)    \
	c->state = __LINE__; \
	return val;          \
	case __LINE__:       \
	;

	switch(c->state) {
	case 0:
		std::cout << "Coroutine: Starting" << std::endl;
		co_yield(true)
		std::cout << "Coroutine: Executing body" << std::endl;
		co_yield(true)
		std::cout << "Coroutine: Finalizing" << std::endl;
		co_yield(false)
	}
	return false;
#undef co_yield
}

int main(int argc, char *argv[])
{
	std::cout << "Main: Tring to start a coroutine" << std::endl;
	Resumable coro;
	coro.resume();
	std::cout << "Main: Doing other useful work" << std::endl;
	coro.resume();
	std::cout << "Main: Preparing final result" << std::endl;
	coro.resume();
}
