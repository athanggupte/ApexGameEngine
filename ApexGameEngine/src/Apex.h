#pragma once

//-----Core-----------------------------
#include "Apex/Application.h"
#include "Apex/Core/AssetManager/AssetManager.h"
#include "Apex/Core/Layers/Layer.h"
#include "Apex/Core/Log.h"
#include "Apex/Core/Timer.h"
// Entity-Component-System
#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/Components.h"
//------------------------------------//

//-----Utility--------------------------
#include "Apex/Utils/ComputeShader/ComputeShader.h"
#include "Apex/Utils/MathPrimitiveParser.h"
#include "Apex/Utils/Profiler.h"
#include "Apex/Utils/Random.h"
#include "Apex/Utils/Utils.h"
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
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Graphics/RenderPrimitives/Buffer.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Framebuffer.h"

#include "Apex/Core/Camera.h"
#include "Apex/Core/CameraController.h"

#include "Apex/Graphics/Renderer/RenderCommands.h"
#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Graphics/Renderer/Renderer2D.h"
//------------------------------------//

//-----PostProcessing-------------------
#include "Apex/Graphics/PostProcessing/PostProcess.h"
#include "Apex/Graphics/PostProcessing/Blur.h"
//------------------------------------//

// #include "Apex/StartUp.h"
