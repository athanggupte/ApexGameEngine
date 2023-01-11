#pragma once

namespace {

	static GLbitfield GetGLFramebufferTargetMask(FramebufferTargetMask mask)
	{
		GLbitfield bitfield = (mask & FramebufferTargetBit::COLOR) ? GL_COLOR_BUFFER_BIT : 0;
		bitfield |= (mask & FramebufferTargetBit::DEPTH) ? GL_DEPTH_BUFFER_BIT : 0;
		bitfield |= (mask & FramebufferTargetBit::STENCIL) ? GL_STENCIL_BUFFER_BIT : 0;
		return bitfield;
	}

}