#pragma once

#include "ConcurrentQueue.h"

#include <QVariant>
#include <QString>
#include <future>
#include <variant>
#include <functional>
#include <memory>

namespace ProtocolEnums {
Q_NAMESPACE

enum class FileStatus {
	Success,
	NotFound,
	AlreadyClosed,
	AlreadyOpened
};
Q_ENUM_NS(FileStatus)

enum class ReportType {
	OpenFile,
	CloseFile,
	GetFile,
	GetFileResult
};
Q_ENUM_NS(ReportType)

}

using FileStatus = ProtocolEnums::FileStatus;
using ReportType = ProtocolEnums::ReportType;

struct OpenFile {
	int index = 0;
};

struct CloseFile {
	int index = 0;
};

struct GetFile {
	int index = 0;
};

struct GetFileResult {
	FileStatus status;
	QString data;
};

using Command = std::variant<OpenFile, CloseFile, GetFile>;
using Reply = std::variant<FileStatus, GetFileResult>;

using Report = std::variant<Command, Reply>;

class DeviceInterface
{
	Q_GADGET
public:
	enum class IOStatus
	{
		Success,
		UnknownError,
		Timeout
	};
	Q_ENUM(IOStatus)

	struct ReplyResult {
		Report reply;
		IOStatus status;
	};
	using UPtr = std::unique_ptr<DeviceInterface>;

	virtual ~DeviceInterface() = default;
	virtual std::future<IOStatus> sendReport(const Report &report, std::function<void(IOStatus)> onFinished) = 0;
	virtual std::future<ReplyResult> getReply(std::function<void(const ReplyResult &)> onFinished) = 0;
};
