#pragma once

#include "CoreMinimal.h"

class FSerialization
{
public:
	static TArray64<uint8> Array2DToNpy(const TArray<FFloat16>& Array, int32 Width, int32 Height);
	static TArray64<uint8> ImageToPng(const TArray<FColor>& Image, int32 Width, int32 Height);
};
