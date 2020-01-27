#pragma once

#include "DeviceInterface.h"

#include <chrono>
#include <unordered_map>

class FakeDevice : public DeviceInterface
{
public:
	FakeDevice();
	~FakeDevice() override;

	std::future<IOStatus> sendReport(const Report &report, std::function<void(IOStatus)> onFinished) override;
	std::future<ReplyResult> getReply(std::function<void(const ReplyResult &)> onFinished) override;

	ReplyResult runCommand(const Command &cmd, IOStatus &cmdStatus);
	ReplyResult run(const OpenFile &cmd, IOStatus &cmdStatus);
	ReplyResult run(const CloseFile &cmd, IOStatus &cmdStatus);
	ReplyResult run(const GetFile &cmd, IOStatus &cmdStatus);

private:
	struct FileInfo {
		QString m_data;
		bool m_isOpened = false;
		std::chrono::milliseconds m_latency = std::chrono::milliseconds(10);
	};

	using FilesContainer = std::unordered_map<int, FileInfo>;
	FileInfo *getFile(int index);

	ConcurrentQueue<ReplyResult> m_replies;
	FilesContainer m_files;
};

namespace conversions {
QString toString(const Report &val);
QString toString(const Command &val);
QString toString(const Reply &val);
QString toString(const OpenFile &val);
QString toString(const CloseFile &val);
QString toString(const GetFile &val);
QString toString(const GetFileResult &val);
QString toString(const DeviceInterface::ReplyResult &val);

template<typename T>
QString toString(T val) {
	return QVariant::fromValue(val).toString();
}
}

