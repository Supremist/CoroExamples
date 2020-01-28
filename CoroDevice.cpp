#include "CoroDevice.h"

#include "FakeDevice.h"
#include <QDebug>
#include <QCoreApplication>

CoroDevice::CoroDevice(DeviceInterface::UPtr io)
	: m_io(std::move(io))
{
}

cppcoro::task<QString> CoroDevice::getFileTask(int fileId)
{
	auto status = std::get<FileStatus>(co_await sendCommand(OpenFile{fileId}));
	checkFileStatus(status);
	auto result = std::get<GetFileResult>(co_await sendCommand(GetFile{fileId}));
	checkFileStatus(result.status);
	status = std::get<FileStatus>(co_await sendCommand(CloseFile{fileId}));
	checkFileStatus(status);
	co_return result.data;
}

AsyncFuture CoroDevice::asyncGetFile(int fileId)
{
	auto fileData = co_await getFileTask(fileId);
	co_return fileData;
}

void CoroDevice::startReadFile(int fileId)
{
	auto future = asyncGetFile(fileId);
	connect(future.signalsEmiter(), &AsyncFutureSignals::success, this, &CoroDevice::success);
	connect(future.signalsEmiter(), &AsyncFutureSignals::failed, this, &CoroDevice::failed);
	future.start();
}

CommandAwaiter CoroDevice::sendReport(const Report &cmd)
{
	return CommandAwaiter(*m_io, cmd);
}

ReplyAwaiter CoroDevice::getReply()
{
	return ReplyAwaiter(*m_io);
}

cppcoro::task<Reply> CoroDevice::sendCommand(const Command &cmd)
{
	auto status = co_await sendReport(cmd);
	checkIoStatus(status);
	auto result = co_await getReply();
	checkIoStatus(result.status);
	co_return std::get<Reply>(result.reply);
}

CommandAwaiter::CommandAwaiter(DeviceInterface &io, const Report &cmd)
	: ReportAwaiter<DeviceInterface::IOStatus>(io)
	, m_cmd(cmd)
{
}

void CommandAwaiter::await_suspend(std::experimental::coroutine_handle<> current)
{
	m_future = m_io.sendReport(m_cmd, [this, current] (DeviceInterface::IOStatus status) {
		QMetaObject::invokeMethod(qApp, [=] {
			m_result = status;
			current.resume();
		});
	});
}

void ReplyAwaiter::await_suspend(std::experimental::coroutine_handle<> current)
{
	m_future = m_io.getReply([this, current] (DeviceInterface::ReplyResult result) {
		QMetaObject::invokeMethod(qApp, [=] {
			m_result = result;
			current.resume();
		});
	});
}

AsyncFuture::AsyncFuture(AsyncFuture::CoroHandle handle)
	: m_handle(handle)
{
}

AsyncFuture::AsyncFuture(AsyncFuture &&other)
{
	std::swap(this->m_handle, other.m_handle);
}

void AsyncFuture::start()
{
	m_handle.resume();
}

AsyncFutureSignals *AsyncFuture::signalsEmiter() const
{
	return &m_handle.promise().emiter;
}


AsyncFuture::promise_type::promise_type()
{
}

AsyncFuture AsyncFuture::promise_type::get_return_object()
{
	return AsyncFuture(AsyncFuture::CoroHandle::from_promise(*this));
}

void AsyncFuture::promise_type::return_value(QString fileData)
{
	emit emiter.success(fileData);
}

void AsyncFuture::promise_type::unhandled_exception()
{
	try {
		throw;
	} catch (const std::exception &ex) {
		emit emiter.failed(QString(ex.what()));
	}
}
