#include "UIResourceAsset.h"
#include "PaperSprite.h"
#include "Engine/AssetManager.h"

void UUIResourceAsset::RequestIconAsync(EUIIconType IconType, TFunction<void(TObjectPtr<UPaperSprite>)> OnLoaded) const
{
	if (const TSoftObjectPtr<UPaperSprite>* Found = IconMap.Find(IconType))
	{
		const FSoftObjectPath Path = Found->ToSoftObjectPath();
		if (Path.IsValid())
		{
			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
			Streamable.RequestAsyncLoad(Path, FStreamableDelegate::CreateLambda([Found, OnLoaded]()
			{
				if (OnLoaded)
				{
					OnLoaded(TObjectPtr<UPaperSprite>(Found->Get()));
				}
			}));
			return;
		}
	}

	if (OnLoaded)
	{
		OnLoaded(nullptr);
	}
}

FSlateBrush UUIResourceAsset::MakeBrushFromSprite(UPaperSprite* Sprite, int32 Width, int32 Height)
{
	if ( Sprite )
	{
		const FSlateAtlasData SpriteAtlasData = Sprite->GetSlateAtlasData();
		const FVector2D SpriteSize = SpriteAtlasData.GetSourceDimensions();

		FSlateBrush Brush;
		Brush.SetResourceObject(Sprite);
		Width = ( Width > 0 ) ? Width : SpriteSize.X;
		Height = ( Height > 0 ) ? Height : SpriteSize.Y;
		Brush.ImageSize = FVector2D(Width, Height);
		return Brush;
	}

	return static_cast<FSlateBrush>(FSlateNoResource());
}
