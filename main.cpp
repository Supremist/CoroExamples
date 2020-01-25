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

	const char * next();

	std::shared_ptr<CoroutineFrame> c;
};

Resumable::Resumable()
    : c(std::make_shared<CoroutineFrame>())
{
	c->state = 0;
}

const char * Resumable::next()
{
#define co_yield(val)    \
	c->state = __LINE__; \
	return val;          \
	case __LINE__:       \
	;

	switch(c->state) {
	case 0:
		co_yield("Coroutine: Starting")
		co_yield("Coroutine: Executing body")
		co_yield("Coroutine: Finalizing")
	}
	return nullptr;
#undef co_yield
}

int main(int argc, char *argv[])
{
	std::cout << "Main: Tring to start a coroutine" << std::endl;
	Resumable coro;
	std::cout << coro.next() << std::endl;
	std::cout << "Main: Doing other useful work" << std::endl;
	std::cout << coro.next() << std::endl;
	std::cout << "Main: Preparing final result" << std::endl;
	std::cout << coro.next() << std::endl;
}
