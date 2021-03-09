#pragma once

//-----Core-----------------------------
#include "Apex/Application.h"
			/*-------------Testing----------------*/
			#include "Apex/Core/ECS/ECSRegistry.h"
			/*------------------------------------*/
#include "Apex/AssetManager/AssetManager.h"
#include "Apex/Core/Layers/Layer.h"
#include "Apex/Core/Log.h"
#include "Apex/Core/Timer.h"
#include "Apex/Core/Random.h"
//------------------------------------//

//-----Utility--------------------------
#include "Apex/Utils/ComputeShader/ComputeShader.h"
#include "Apex/Utils/MathPrimitiveParser.h"
#include "Apex/Utils/Profiler.h"
//------------------------------------//

//-----Input----------------------------
#include "Apex/Core/Input/Input.h"
#include "Apex/Core/Input/KeyCodes.h"
#include "Apex/Core/Input/MouseCodes.h"
//------------------------------------//

//-----ImGui----------------------------
#include "Apex/ImGui/ImGuiLayer.h"
//------------------------------------//

//-----Renderer-------------------------
#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Graphics/Renderer/RenderCommands.h"

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Graphics/RenderPrimitives/Buffer.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/FrameBuffer.h"

#include "Apex/Core/Camera.h"
//------------------------------------//

//-----PostProcessing-------------------
#include "Apex/Graphics/PostProcessing/Blur.h"
//------------------------------------//

#include "Apex/StartUp.h" 