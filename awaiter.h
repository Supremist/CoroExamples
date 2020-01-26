#pragma once

struct coroutine_handle;

struct awaiter {
	bool await_ready();

	// must schedule resumption of the current_coro when this will be ready
	// may return void, bool or another coroutine_handle
	auto await_suspend(coroutine_handle current_coro);

	// becomes the result of whole "co_await <expression>;" statement
	auto await_resume();
};

auto get_awaitable(auto expression)
{
	if (produced_by_init_suspend(expression)
	        || produced_by_final_suspend(expression)
	        || produced_by_co_yield(expression)) {
		return expression;
	}

	if (has_member(promise_type, await_transform)) {
		return promise.await_transform(expression);
	}

	return expression;
}

auto get_awaiter(expression)
{
	auto awaitable = get_awaitable(expression);
	if (has_overload(awaitable, operator co_await)) {
		return operator co_await(static_cast<Awaitable&&>(awaitable));
	}
	return awaitable;
}

coroutine_handle get_next_coroutine(awaiter _awaiter, coroutine_handle current_coro)
{
	if (bool(_awaiter.await_ready())) {
		return current_coro;
	}

	try {
		auto suspend_expr = _awaiter.await_suspend(current_coro);
	} catch(...) {
		remember_current_exception();
		return current_coro;
	}

	if (is_same(decltype(suspend_expr), void)) {
		return std::noop_coroutine_handle;
	}

	if (is_same(decltype(suspend_expr), bool)) {
		return suspend_expr ? std::noop_coroutine_handle : current_coro;
	}

	return suspend_expr;
}

auto await_block(expression)
{
	auto awaiter = get_awaiter(expression);
	auto next_coro = get_next_coroutine(awaiter, current_corotine);
	if (next_coro == std::noop_coroutine_handle) {
		return_to_the_caller();
	} else if (next_coro == current_corotine) {
		if (has_remembered_exception()) {
			rethrow_remembered_exception();
		}
		return awaiter.await_resume();
	} else {
		next_coro.resume();
		return_to_the_caller();
	}
}
