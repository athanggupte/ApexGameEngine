#pragma once

#include "Texture.h"

namespace Apex {

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AttachTexture(const Ref<Texture2D>& texture) = 0;

		virtual const std::vector<Ref<Texture>>& GetTextures() const = 0;

		static Ref<FrameBuffer> Create(bool depth);
	};

	class DepthBuffer
	{
	public:
		virtual ~DepthBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const Ref<TextureDepth2D>& GetDepthTexture() const = 0;

		static Ref<DepthBuffer> Create();
	};

}