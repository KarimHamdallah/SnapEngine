#pragma once

#include <Snap/Core/Application.h>

//----- Engine Systems ------
#include <Snap/Core/Core.h>
#include <Snap/Core/log.h>
#include <Snap/Core/Layer.h>
#include <Snap/ImGui/ImGuiLayer.h>
#include <Snap/Core/Input.h>
#include <Snap/Core/TimeStep.h>
#include <Snap/Core/Timer.h>

#include <imgui.h>

//------ Events --------------
#include <Snap/Events/KeyEvent.h>
#include <Snap/Events/MouseEvent.h>
#include <Snap/Events/WindowEvent.h>

//------ Math ----------------
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//------ Renderer ------------
#include <Snap/Renderer/Renderer.h>
#include <Snap/Renderer/Renderer2D.h>
#include <Snap/Renderer/RendererCommand.h>
#include <Snap/Renderer/RendererAPI.h>

#include <Snap/Renderer/Buffer.h>
#include <Snap/Renderer/VertexArray.h>

#include <Snap/Renderer/Shader.h>
#include <Snap/Renderer/Texture.h>
#include <Snap/Renderer/SubTexture2D.h>

#include <Snap/Renderer/OrthoGraphicCamera.h>
#include <Snap/Renderer/OrthoGraphicCameraController.h>

//---------------------------

//------ EntryPoint ---------
#include <Snap/Core/EntryPoint.h>
//---------------------------