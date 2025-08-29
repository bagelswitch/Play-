#pragma once

#include <list>
#include "GSHandler.h"
#include "GsCachedArea.h"

#define TEX0_CLUTINFO_MASK (~0xFFFFFFE000000000ULL)

template <typename TextureHandleType>
class CGsTextureCache
{
public:
	class CTexture
	{
	public:
		void Reset()
		{
			m_live = false;
			m_textureHandle = TextureHandleType();
			m_cachedArea.ClearDirtyPages();
		}

		uint64 m_tex0 = 0;
		bool m_live = false;
		CGsCachedArea m_cachedArea;

		//Platform specific
		TextureHandleType m_textureHandle;
	};

	CTexture* Search(const CGSHandler::TEX0& tex0)
	{
		uint64 key = static_cast<uint64>(tex0) & TEX0_CLUTINFO_MASK;

		auto it = m_textureMap.find(key);
		if(it != m_textureMap.end())
		{
			auto result = it->second;
			return result.get();
		}
		else {
			return nullptr;
		}
	}

	void Insert(const CGSHandler::TEX0& tex0, TextureHandleType textureHandle)
	{
		uint64 key = static_cast<uint64>(tex0) & TEX0_CLUTINFO_MASK;

		uint32 bufSize = tex0.GetBufWidth();
		if(bufSize == 0)
		{
			bufSize = std::min<uint32>(tex0.GetWidth(), CGSHandler::TEX0_MAX_TEXTURE_SIZE);
		}
		uint32 texHeight = std::min<uint32>(tex0.GetHeight(), CGSHandler::TEX0_MAX_TEXTURE_SIZE);

		auto texture = std::make_shared<CTexture>();
		texture->m_cachedArea.SetArea(tex0.nPsm, tex0.GetBufPtr(), bufSize, texHeight);
		texture->m_tex0 = key;
		texture->m_textureHandle = std::move(textureHandle);
		texture->m_live = true;

		m_textureMap[key] = texture;
	}

	void InvalidateRange(uint32 start, uint32 size)
	{
		for(auto it = m_textureMap.cbegin(); it != m_textureMap.cend(); it++)
		{
			it->second->m_cachedArea.Invalidate(start, size);
		}
	}

	void Flush()
	{
		for(auto it = m_textureMap.cbegin(); it != m_textureMap.cend() /* not hoisted */; /* no increment */)
		{
			it->second->Reset();
			m_textureMap.erase(it++);
		}
	}

private:
	typedef std::shared_ptr<CTexture> TexturePtr;
	typedef std::unordered_map<uint64, TexturePtr> TextureMap;
	TextureMap m_textureMap;
};
