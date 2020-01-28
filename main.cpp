#include "FakeDevice.h"
#include "CoroDevice.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <iostream>
#include <memory>

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

void readBlocking()
{
	DeviceInterface::UPtr device = std::make_unique<FakeDevice>();
	try {
		qDebug() << "Read file data:" << getFile(*device, 10);
	} catch (const std::exception &ex) {
		qCritical() << "Exception: " << ex.what();
	}
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	CoroDevice asyncReader(std::make_unique<FakeDevice>());
	QObject::connect(&asyncReader, &CoroDevice::success, &a, [&a] (QString fileData){
		qDebug() << "Read file data:" << fileData;
		a.quit();
	});
	QObject::connect(&asyncReader, &CoroDevice::failed, &a, [&a] (QString errorMsg){
		qCritical() << "Exception: " << errorMsg;
		a.quit();
	});

	asyncReader.startReadFile(2);

	int progress = 0;
	QTimer progressTimer;
	progressTimer.setSingleShot(false);
	progressTimer.setInterval(200);
	QObject::connect(&progressTimer, &QTimer::timeout, &a, [&progress] {
		qDebug() << "Progress:" << progress++;
	});
	progressTimer.start();
	return a.exec();
}
