#include "Tasks/SaveTextureToDisk.h"
#include "Modules/ModuleManager.h"
#include "Kismet/KismetRenderingLibrary.h"

AsyncSaveTextureToDiskTask::AsyncSaveTextureToDiskTask(UObject* WorldObj, UTexture2D* TTarget, FString FPath, FString FName) {
	WorldObject = WorldObj;
	TextureTarget = TTarget;
	FilePath = FPath;
	FileName = FName;
}

AsyncSaveTextureToDiskTask::~AsyncSaveTextureToDiskTask() { }

void AsyncSaveTextureToDiskTask::DoWork() {
	UKismetRenderingLibrary::ExportTexture2D(WorldObject, TextureTarget, FilePath, FileName);
}