#pragma once

//For use by applications only

#include "Apex/Application.h"
#include "Apex/AssetManager/AssetManager.h"
#include "Apex/Layers/Layer.h"
#include "Apex/Log/Log.h"
#include "Apex/Utils/MathPrimitiveParser.h"
#include "Apex/Utils/Timer.h"
#include "Apex/Utils/Random.h"

//-----Input----------------------------
#include "Apex/Input/Input.h"
#include "Apex/Input/KeyCodes.h"
#include "Apex/Input/MouseCodes.h"
//------------------------------------//

//-----ImGui----------------------------
#include "Apex/ImGui/ImGuiLayer.h"
//------------------------------------//

//-----Renderer-------------------------
#include "Apex/Renderer/Renderer.h"
#include "Apex/Renderer/RenderCommands.h"

#include "Apex/RenderPrimitives/Shader.h"
#include "Apex/RenderPrimitives/Texture.h"
#include "Apex/RenderPrimitives/Buffer.h"
#include "Apex/RenderPrimitives/VertexArray.h"
#include "Apex/RenderPrimitives/FrameBuffer.h"
#include "Apex/RenderPrimitives/Camera.h"
//------------------------------------//

//-----PostProcessing-------------------
#include "Apex/PostProcessing/Blur.h"
//------------------------------------//

#include "Apex/StartUp.h" 