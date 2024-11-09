// Weichao Qiu @ 2018
#include "Utils/Serialization.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"

#include "ThirdParty/cnpy.h"

TArray64<uint8> FSerialization::Array2DToNpy(const TArray<FFloat16>& Array, int32 Width, int32 Height)
{
	float *TypePointer = nullptr; // Only used for determing the type

	std::vector<int> Shape;
	Shape.push_back(Height);
	Shape.push_back(Width);

	std::vector<char> NpyHeader = cnpy::create_npy_header(TypePointer, Shape);

	std::vector<float> FloatData;

	for (int i = 0; i < Array.Num(); i++) {
		FloatData.push_back(Array[i]);
	}
	const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&FloatData[0]);
	std::vector<unsigned char> NpyData(bytes, bytes + sizeof(float) * FloatData.size());
	NpyHeader.insert(NpyHeader.end(), NpyData.begin(), NpyData.end());

	TArray64<uint8> BinaryData;
	for (char Element : NpyHeader)
	{
		BinaryData.Add(Element);
	}
	return BinaryData;
}

TArray64<uint8> FSerialization::ImageToPng(const TArray<FColor>& Image, int32 Width, int32 Height)
{
	if (Image.Num() == 0 || Image.Num() != Width * Height)
	{
		return TArray64<uint8>();
	}
	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	ImageWrapper->SetRaw(Image.GetData(), Image.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8);
	const TArray64<uint8>& ImgData = ImageWrapper->GetCompressed();
	return ImgData;
}