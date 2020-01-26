#pragma once

struct promise_type
{
  resumable get_return_object() {/**/}
  awaitable initial_suspend() {/**/}
  awaitable final_suspend() {/**/}

  void return_void() {/**/}
  void return_value(value_type val) {/**/}
};
