#include <iostream>
#include <memory>

struct CoroutineFrame
{
	enum class State {
		Initial,
		Body,
		Final
	};

	State state;
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
	c->state = CoroutineFrame::State::Initial;
}

bool Resumable::resume()
{
	switch(c->state) {
	case CoroutineFrame::State::Initial:
		std::cout << "Coroutine: Starting" << std::endl;
		c->state = CoroutineFrame::State::Body;
		return true;
	case CoroutineFrame::State::Body:
		std::cout << "Coroutine: Executing body" << std::endl;
		c->state = CoroutineFrame::State::Final;
		return true;
	case CoroutineFrame::State::Final:
		std::cout << "Coroutine: Finalizing" << std::endl;
		return false;
	}
	return false;
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
