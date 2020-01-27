#pragma once

#include "DeviceInterface.h"

#include <QObject>

class AsyncDevice : public QObject
{
	Q_OBJECT
public:
	AsyncDevice(DeviceInterface::UPtr io);

	void startReadFile(int fileId);

signals:
	void failed(QString error);
	void success(QString fileData);

private:
	void onOpenFinished(FileStatus status);
	void onReadFinished(GetFileResult result);
	void onCloseFinished(FileStatus status);

	template<typename ReplyType>
	void sendCommand(const Command &cmd, void (AsyncDevice::*onReplyReceived)(ReplyType));

	int m_fileId;
	QString m_fileData;
	DeviceInterface::UPtr m_io;
	std::future<DeviceInterface::IOStatus> m_lastCommand;
	std::future<DeviceInterface::ReplyResult> m_lastReply;
};


