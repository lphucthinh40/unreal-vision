#include "Tasks/SaveImageToDisk.h"
#include "Modules/ModuleManager.h"

AsyncSaveImageToDiskTask::AsyncSaveImageToDiskTask(TArray64<uint8> Image, FString ImageName) {
	ImageCopy = Image;
	FileName = ImageName;
}

AsyncSaveImageToDiskTask::~AsyncSaveImageToDiskTask() { }

void AsyncSaveImageToDiskTask::DoWork() {
	FFileHelper::SaveArrayToFile(ImageCopy, *FileName);
}
