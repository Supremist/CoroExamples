#include "FakeDevice.h"

#include <iostream>
#include <memory>
#include <QDebug>

void checkIoStatus(DeviceInterface::IOStatus ioStatus) {
	if (ioStatus != DeviceInterface::IOStatus::Success) {
		throw std::logic_error(conversions::toString(ioStatus).toStdString());
	}
}

void checkFileStatus(FileStatus fileStatus) {
	if (fileStatus != FileStatus::Success) {
		throw std::logic_error(conversions::toString(fileStatus).toStdString());
	}
}

Reply sendCommand(DeviceInterface &device, const Command &cmd)
{
	checkIoStatus(device.sendReport(cmd, {}).get());
	auto result = device.getReply({}).get();
	checkIoStatus(result.status);
	return std::get<Reply>(result.reply);
}

QString getFile(DeviceInterface &device, int fileId)
{
	auto status = std::get<FileStatus>(sendCommand(device, OpenFile{fileId}));
	checkFileStatus(status);
	auto result = std::get<GetFileResult>(sendCommand(device, GetFile{fileId}));
	checkFileStatus(result.status);
	status = std::get<FileStatus>(sendCommand(device, CloseFile{fileId}));
	checkFileStatus(status);
	return result.data;
}

int main(int argc, char *argv[])
{
	DeviceInterface::UPtr device = std::make_unique<FakeDevice>();
	try {
		qDebug() << "Read file data:" << getFile(*device, 10);
	} catch (const std::exception &ex) {
		qCritical() << "Exception: " << ex.what();
	}
}
