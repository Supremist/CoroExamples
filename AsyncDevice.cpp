#include "AsyncDevice.h"
#include "FakeDevice.h"

AsyncDevice::AsyncDevice(DeviceInterface::UPtr io)
	: m_io(std::move(io))
{
}

void AsyncDevice::startReadFile(int fileId)
{
	m_fileId = fileId;
	m_fileData = QString();
	sendCommand<FileStatus>(OpenFile{fileId}, &AsyncDevice::onOpenFinished);
}

void AsyncDevice::onOpenFinished(FileStatus status)
{
	if (status != FileStatus::Success) {
		emit this->failed(conversions::toString(status));
		return;
	}

	sendCommand<GetFileResult>(GetFile{m_fileId}, &AsyncDevice::onReadFinished);
}

void AsyncDevice::onReadFinished(GetFileResult result)
{
	if (result.status != FileStatus::Success) {
		emit this->failed(conversions::toString(result.status));
		return;
	}

	m_fileData = result.data;
	sendCommand<FileStatus>(CloseFile{m_fileId}, &AsyncDevice::onCloseFinished);
}

void AsyncDevice::onCloseFinished(FileStatus status)
{
	if (status != FileStatus::Success) {
		emit this->failed(conversions::toString(status));
		return;
	}
	emit success(m_fileData);
}

template<typename ReplyType>
void AsyncDevice::sendCommand(const Command &cmd, void (AsyncDevice::*onReplyReceived)(ReplyType))
{
	m_lastCommand = m_io->sendReport(cmd, [this, onReplyReceived] (DeviceInterface::IOStatus status) {
		if (status != DeviceInterface::IOStatus::Success) {
			emit this->failed(conversions::toString(status));
			return;
		}

		m_lastReply = m_io->getReply([this, onReplyReceived] (const DeviceInterface::ReplyResult &result) {
			if (result.status != DeviceInterface::IOStatus::Success) {
				emit this->failed(conversions::toString(result.status));
				return;
			}

			try {
				auto reply = std::get<Reply>(result.reply);
				QMetaObject::invokeMethod(this, [this, onReplyReceived, concreteReply = std::get<ReplyType>(reply)] {
					(this->*onReplyReceived)(concreteReply);
				});
			} catch (const std::exception &ex) {
				emit this->failed(QString(ex.what()));
			}
		});
	});
}
