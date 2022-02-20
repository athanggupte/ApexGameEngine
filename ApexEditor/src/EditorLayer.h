#pragma once
#include "Apex/Core/Layers/Layer.h"

//#include <irrKlang.h>

#include "Apex/Graphics/ShadowMap.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/LogPanel.h"
#include "Panels/AssetExplorer.h"
#include "Panels/MaterialPanel.h"
#include "Panels/ResourceViewer.h"

#include <glm/glm.hpp>

#ifndef APEX_INSTALL_LOCATION
#error Install location not specified
#endif

namespace Apex {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		void OnImGuiRender() override;
		
		// UI
		/* UI function naming convention
		 *  - BeginXXX and EndXXX for containers that encompass varied number of child elements
		 *  - ShowXXX for Windows, Bars and other self-complete elements
		 *  - DrawXXX for embedded elements/parts of complete elements
		 */
		void BeginDockspace();
		void EndDockspace();
		void ShowSettings();
		void GameViewport();
		void DrawGizmos();
		void Logger();
		void MainMenuBar();
		void SceneToolbar();
		void DrawFileMenu();
		void DrawEditMenu();
		void ComputeShaderOutput();

	protected:
		// Event Functions
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		// Scene functions
		void SceneNew();
		void SceneOpen();
		void SceneOpen(const fs::path& path);
		void SceneSave();
		void SceneSaveAs();
		void SceneSaveAs(const fs::path& path) const;

		// Scene State functions
		void OnScenePlay();
		void OnSceneStop();
		
	private:
		enum class CameraMode
		{
			ORTHOGRAPHIC,
			ISOMETRIC,
			PERSPECTIVE,
		};

		
		enum class SceneState
		{
			Edit,
			Play,
			Simulate,

			Transition_StartPlay,
			Transition_StopPlay,

		} m_SceneState = SceneState::Edit;

		// State
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		
		bool m_Open;
		bool m_ShowNodeGraph = true;

		glm::vec4 m_BGColor;

		// Editor Resources
		Ref<Framebuffer> m_GameFramebuffer;
		Ref<Framebuffer> m_GameFramebufferMS;
		Ref<Framebuffer> m_PostProcessFramebuffer;
		glm::vec2 m_GameViewportSize;
		glm::vec2 m_GameViewportPos;

		Camera m_EditorCamera;
		OrthographicCameraController2D m_OrthographicCameraController;
		PerspectiveCameraController m_PerspectiveCameraController;
		CameraController* m_EditorCameraController;

		Ref<Scene> m_ActiveScene, m_EditorScene;
		Ref<ResourceManager> m_ResourceManagerSnapshot;

		Ref<Texture2D> m_Texture;
		Ref<Texture2D> m_ImageTexture;		
		Ref<ComputeShader> m_ComputeShader;

		// Panel data
		std::shared_ptr<EditorLogSink_mt> m_LogSink;
		LogPatternFlags_t m_LogPatternFlags = LogPattern::None;
		bool m_LogPatternChanged = false;
		LogPanel m_LogPanel;
		
		SceneHierarchyPanel m_SceneHierarchyPanel;
		InspectorPanel m_InspectorPanel;
		AssetExplorer m_AssetExplorer;
		MaterialPanel m_MaterialPanel;
		ResourceViewer m_ResourceViewer;

		::SingleCopyStack<std::string, 16> m_RecentFiles;

		// Editor assets
		Ref<Texture> m_IconEditor;

		Ref<Shader> m_ShaderGrid;
		int m_GizmoType = -1;


		// Testing
		// TODO: Refactor and rearrange these members
		Ref<ShadowMap> m_ShadowMap;

		//irrklang::ISoundEngine* m_SoundEngine;
		//irrklang::ISound* m_Sound;
	};

}
