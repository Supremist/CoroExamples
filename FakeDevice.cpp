#include "FakeDevice.h"

#include <QtGlobal>
#include <QDebug>

FakeDevice::FakeDevice()
{
	using namespace std::chrono_literals;
	m_files[2] = FileInfo{QString("Hello World"), false, 10ms};
	m_files[10] = FileInfo{QString("A very slow file"), false, 1000ms};
}

FakeDevice::~FakeDevice()
{
	for (const auto &filePair : m_files) {
		Q_ASSERT(!filePair.second.m_isOpened);
	}
}

std::future<DeviceInterface::IOStatus> FakeDevice::sendReport(const Report &report, std::function<void (DeviceInterface::IOStatus)> onFinished)
{
	return std::async(std::launch::async, [this, report, onFinished] {
		DeviceInterface::IOStatus cmdStatus = DeviceInterface::IOStatus::Success;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		qDebug() << conversions::toString(report);
		m_replies.push(runCommand(std::get<Command>(report), cmdStatus));
		if (onFinished) {
			onFinished(cmdStatus);
		}
		return cmdStatus;
	});
}

std::future<DeviceInterface::ReplyResult> FakeDevice::getReply(std::function<void (const DeviceInterface::ReplyResult &)> onFinished)
{
	return std::async(std::launch::async, [this, onFinished] {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		auto result = m_replies.popWait().value();
		qDebug() << conversions::toString(result);
		if (onFinished) {
			onFinished(result);
		}
		return result;
	});
}

DeviceInterface::ReplyResult FakeDevice::runCommand(const Command &cmd, DeviceInterface::IOStatus &cmdStatus)
{
	return std::visit([this, &cmdStatus] (auto concreteCommand) {
		return run(concreteCommand, cmdStatus);
	}, cmd);
}

DeviceInterface::ReplyResult FakeDevice::run(const OpenFile &cmd, DeviceInterface::IOStatus &cmdStatus)
{
	auto file = getFile(cmd.index);
	if (!file) {
		return ReplyResult{Reply(FileStatus::NotFound), IOStatus::Success};
	}

	if (file->m_isOpened) {
		return ReplyResult{Reply(FileStatus::AlreadyOpened), IOStatus::Success};
	}

	file->m_isOpened = true;
	return ReplyResult{Reply(FileStatus::Success), IOStatus::Success};
}

DeviceInterface::ReplyResult FakeDevice::run(const CloseFile &cmd, DeviceInterface::IOStatus &cmdStatus)
{
	auto file = getFile(cmd.index);
	if (!file) {
		return ReplyResult{Reply(FileStatus::NotFound), IOStatus::Success};
	}

	if (!file->m_isOpened) {
		return ReplyResult{Reply(FileStatus::AlreadyClosed), IOStatus::Success};
	}

	file->m_isOpened = false;
	return ReplyResult{Reply(FileStatus::Success), IOStatus::Success};
}

DeviceInterface::ReplyResult FakeDevice::run(const GetFile &cmd, DeviceInterface::IOStatus &cmdStatus)
{
	auto file = getFile(cmd.index);
	if (!file) {
		return ReplyResult{Reply(GetFileResult{FileStatus::NotFound, {}}), IOStatus::Success};
	}

	if (!file->m_isOpened) {
		return ReplyResult{Reply(GetFileResult{FileStatus::AlreadyClosed, {}}), IOStatus::Success};
	}

	return ReplyResult{Reply(GetFileResult{FileStatus::Success, file->m_data}), IOStatus::Success};
}

FakeDevice::FileInfo *FakeDevice::getFile(int index)
{
	auto file = m_files.find(index);
	if (file == m_files.end()) {
		return nullptr;
	}

	std::this_thread::sleep_for(file->second.m_latency);
	return &file->second;
}

QString conversions::toString(const Report &val)
{
	return std::visit([](auto value) {
		return conversions::toString(value);
	}, val);
}

QString conversions::toString(const Command &val)
{
	return std::visit([](auto value) {
		return QString("Command ") + conversions::toString(value);
	}, val);
}

QString conversions::toString(const Reply &val)
{
	return std::visit([](auto value) {
		return QString("Reply ") + conversions::toString(value);
	}, val);
}

QString conversions::toString(const OpenFile &val)
{
	return QString("OpenFile{%1}").arg(val.index);
}

QString conversions::toString(const CloseFile &val)
{
	return QString("CloseFile{%1}").arg(val.index);
}

QString conversions::toString(const GetFile &val)
{
	return QString("GetFile{%1}").arg(val.index);
}

QString conversions::toString(const GetFileResult &val)
{
	return QString("GetFileResult{%1, %2}").arg(toString(val.status)).arg(val.data);
}

QString conversions::toString(const DeviceInterface::ReplyResult &val)
{
	return QString("ReplyResult{%1, %2}").arg(toString(val.status)).arg(toString(val.reply));
}
