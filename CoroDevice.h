#pragma once

#include "DeviceInterface.h"

#include <cppcoro/task.hpp>
#include <QObject>
#include <experimental/coroutine>
#include <stdexcept>

template<typename Result>
class ReportAwaiter
{
public:
	ReportAwaiter(DeviceInterface &io)
		: m_io(io)
	{
	}

	bool await_ready() const noexcept { return false; }
	Result await_resume() const
	{
		return m_result;
	}

protected:
	DeviceInterface &m_io;
	Result m_result;
	std::future<Result> m_future;
};

class CommandAwaiter : public ReportAwaiter<DeviceInterface::IOStatus>
{
public:
	CommandAwaiter(DeviceInterface &io, const Report &cmd);
	void await_suspend(std::experimental::coroutine_handle<> current);

private:
	Report m_cmd;
};

class ReplyAwaiter : public ReportAwaiter<DeviceInterface::ReplyResult>
{
public:
	using ReportAwaiter<DeviceInterface::ReplyResult>::ReportAwaiter;
	void await_suspend(std::experimental::coroutine_handle<> current);
};

class AsyncFutureSignals : public QObject
{
	Q_OBJECT
public:
	using QObject::QObject;

signals:
	void failed(QString error);
	void success(QString fileData);
};

class AsyncFuture
{
public:
	struct promise_type
	{
		promise_type();

		AsyncFuture get_return_object();

		auto initial_suspend() { return std::experimental::suspend_always(); }
		auto final_suspend() { return std::experimental::suspend_always(); }

		void return_value(QString fileData);

		void unhandled_exception();

		AsyncFutureSignals emiter;
	};

	using CoroHandle = std::experimental::coroutine_handle<AsyncFuture::promise_type>;
	AsyncFuture(CoroHandle handle);
	AsyncFuture(AsyncFuture &&other);

	void start();
	AsyncFutureSignals *signalsEmiter() const;
private:
	CoroHandle m_handle;
};

class CoroDevice : public QObject
{
	Q_OBJECT

public:
	CoroDevice(DeviceInterface::UPtr io);

	cppcoro::task<QString> getFileTask(int fileId);
	AsyncFuture asyncGetFile(int fileId);
	void startReadFile(int fileId);

signals:
	void failed(QString error);
	void success(QString fileData);

private:
	CommandAwaiter sendReport(const Report &cmd);
	ReplyAwaiter getReply();
	cppcoro::task<Reply> sendCommand(const Command &cmd);

	DeviceInterface::UPtr m_io;
};


