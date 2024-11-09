#pragma once

class AsyncSaveTextureToDiskTask : public FNonAbandonableTask{
public:
	AsyncSaveTextureToDiskTask(UObject* WorldObj, UTexture2D* TTarget, FString FPath, FString FName);
	~AsyncSaveTextureToDiskTask();

	// Required by UE4!
	FORCEINLINE TStatId GetStatId() const{
		RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSaveTextureToDiskTask, STATGROUP_ThreadPoolAsyncTasks);
	}

protected:
	UObject* WorldObject;
	UTexture2D* TextureTarget;
	FString FilePath = "";
	FString FileName = "";

public:
	void DoWork();
};