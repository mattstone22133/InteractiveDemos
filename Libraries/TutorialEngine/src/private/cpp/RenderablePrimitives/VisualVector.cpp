#include "RenderablePrimitives/VisualVector.h"
#include "RenderablePrimitives/LineRenderer.h"
#include "Utils/Shader.h"
#include "Utils/MathUtils.h"
#include "GameObjectBase.h"
#include "Geometry/ConeGenerator.h"
#include "Geometry/ConeMesh_GpuBuffer.h"
//#include "static_mesh.h" //#PORT TODO_REMOVE_AFTER_STATIC_MESH_LOGIC_REMOVED;

using namespace Engine;

namespace TutorialEngine
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// statics
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*static*/int TutorialEngine::VisualVector::numInstances = 0;
	/*static*/sp<Engine::ConeMesh_GpuBuffer> VisualVector::tipMesh = nullptr;
	/*static*/sp<Engine::ConeMesh_GpuBuffer> VisualVector::tipMeshOffset = nullptr;
	/*static*/sp<Engine::Shader> VisualVector::tipShader = nullptr;
	/*static*/sp<TutorialEngine::LineRenderer> VisualVector::lineRenderer = nullptr;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// members
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VisualVector::VisualVector()
	{
		numInstances++;
		if (numInstances == 1)
		{
			Engine::Shader::ShaderParams coneShaderInit;
			coneShaderInit.vertex_src = R"(
					//#version 410 core
					//layout (location = 0) in vec3 position;				
					//layout (location = 1) in vec3 normal;
					//layout (location = 2) in vec2 uv;
					//out vec3 worldPos;
					//out vec3 worldNormal;
					//out vec2 fragUV;

					attribute vec3 position;				
					attribute vec3 normal;
					attribute vec2 uv;

					varying vec3 worldPos;
					varying vec3 worldNormal;
					varying vec2 fragUV;

					uniform highp mat4 model;					// = mat4(1.0);
					uniform highp mat4 normal_matrix;// = mat4(1.0);
					uniform highp mat4 projection_view;		// = mat4(1.0);

					void main(){
						worldPos = (model * vec4(position,1.0)).xyz;
						//worldNormal = normalize((inverse(transpose(model)) * vec4(normal, 0.0)).xyz); //inverse function not available in glsl1 in gles2
						worldNormal = normalize(normal_matrix * vec4(normal, 0.0)).xyz;
						fragUV = uv;

						gl_Position = projection_view * vec4(worldPos, 1.0);
					}
				)";
			coneShaderInit.fragment_src = R"(
					//#version 410 core
					//out vec4 fragmentColor;
					//in vec3 worldPos;
					//in vec3 worldNormal;
					//in vec2 fragUV;

					varying highp vec3 worldPos;
					varying highp vec3 worldNormal;
					varying highp vec2 fragUV;

					uniform highp vec3 dirLight;			// = normalize(vec3(-1,-1,-1));
					uniform highp vec3 solidColor;			// = vec3(1,0,0);
					uniform highp int bUseLight;			// = false;
					//uniform bool bUseTexture;				// = false;
					uniform highp int bUseCameraLight;		// = false;
					uniform highp vec3 cameraPos;			// = vec3(0,0,0);

					//NOTE: I have removed all usage of textures from this during port; if they are needed then perhaps just re-porting this shader would be ismpler.
					void main()
					{
						if(bUseLight != 0)
						{
							highp vec3 toLight_n = bUseCameraLight != 0 ? normalize(cameraPos - worldPos) : normalize(dirLight);
							highp float diffuseFactor = max(dot(toLight_n, worldNormal.xyz), 0.0);

							highp vec3 colorToUse = solidColor.xyz;
							highp vec3 diffuse = colorToUse * diffuseFactor;
							highp vec3 ambient = colorToUse * 0.05;
							gl_FragColor = vec4(ambient+diffuse, 1.0);
						}
						else
						{
							gl_FragColor = vec4(solidColor.rgb, 1.0);
						}
					}
				)";
			coneShaderInit.uniformInitializerFunc = [](Engine::Shader& shader) {
				//VERTEX
				shader.setMat4("model", glm::mat4(1.0f));
				shader.setMat4("normal_matrix", glm::mat4(1.0f));
				shader.setMat4("projection_view", glm::mat4(1.0f));

				//FRAGMENT
				shader.setUniform3f("dirLight", glm::normalize(glm::vec3(-1, -1, -1)));
				shader.setUniform3f("solidColor", glm::vec3(1, 0, 0));
				shader.setUniform1i("bUseLight", 0);
				//shader.setUniform1i("bUseTexture", 0);
				shader.setUniform1i("bUseCameraLight", 0);
				shader.setUniform3f("cameraPos", glm::vec3(0, 0, 0));
			};
			tipShader = new_sp<Engine::Shader>(coneShaderInit);
			lineRenderer = new_sp<TutorialEngine::LineRenderer>();
			//tipMesh = new_sp<StaticMesh::Model>("./assets/models/cone_tip/cone_tip_centered.obj"); 
			//tipMeshOffset = new_sp<StaticMesh::Model>("./assets/models/cone_tip/cone_tip.obj");
			const int32_t coneNumFaces = 25;
			const float coneHeight = 2.0f;
			const float coneRadius = 1.0f;
			const bool bSealCone = true;
			tipMesh = new_sp<ConeMesh_GpuBuffer>(coneNumFaces, coneHeight, coneRadius, ConeGenerator::ConeOriginGeneration::BASE, bSealCone);
			tipMeshOffset = new_sp<ConeMesh_GpuBuffer>(coneNumFaces, coneHeight, coneRadius, ConeGenerator::ConeOriginGeneration::TIP, bSealCone);
			//#todo #static_mesh_refactor verify above looks correct, refactoring away from using static meshes but cannot compile code atm to verify. likely requires iterator.
		}
	}

	VisualVector::VisualVector(const VisualVector& copy)
	{
		if (&copy != this)
		{
			pod = copy.pod;
			color = copy.color;

			numInstances++;
		}
	}


	VisualVector::VisualVector(VisualVector&& move) noexcept
	{
		if (&move != this)
		{
			color = move.color;
			pod = move.pod;

			//need to increment because moving argument will be haves it dtor called
			numInstances++;
		}
	}

	TutorialEngine::VisualVector& VisualVector::operator=(VisualVector&& move) noexcept
	{
		if (&move != this)
		{
			color = move.color;
			pod = move.pod;

			//need to increment because moving argument will be haves it dtor called
			numInstances++;
		}
		return *this;
	}

	void VisualVector::render(const glm::mat4& projection_view, std::optional<glm::vec3> cameraPos) const
	{
		if (!tipShader)
		{
			return;
		}

		glm::vec3 end = pod.startPos + pod.dir;

		lineRenderer->renderLine(
			pod.startPos,
			end,
			color,
			projection_view
		);

		tipShader->use();
		tipShader->setMat4("model", pod.cachedTipXform);
		tipShader->setMat4("normal_matrix", pod.cachedNormalMatrix);
		tipShader->setMat4("projection_view", projection_view);
		tipShader->setUniform3f("solidColor", color);

		if (cameraPos)
		{
			tipShader->setUniform3f("cameraPos", *cameraPos);
			tipShader->setUniform1i("bUseCameraLight", true);
			tipShader->setUniform1i("bUseLight", true);
		}
		else
		{
			tipShader->setUniform1i("bUseCameraLight", true);
			tipShader->setUniform1i("bUseLight", false);
		}

		if (bUseCenteredMesh)
		{
			tipMesh->render(*tipShader, /*activateShader*/ false);
		}
		else
		{
			tipMeshOffset->render(*tipShader, /*activateShader*/ false);
		}
	}

	void VisualVector::setVector(glm::vec3 newVec)
	{
		pod.dir = newVec;
		pod.dir_n = glm::normalize(pod.dir);

		updateCache();
	}

	void VisualVector::setStart(glm::vec3 newStart)
	{
		pod.startPos = newStart;

		updateCache();
	}

	void VisualVector::setEnd(glm::vec3 newEnd)
	{
		glm::vec3 newDir = newEnd - pod.startPos;
		setVector(newDir);
	}

	void VisualVector::updateCache()
	{
		glm::quat tipRotation = MathUtils::getRotationBetween(glm::vec3(0.f, 0.f, 1.f), pod.dir_n);

		glm::vec3 end = pod.startPos + pod.dir;

		pod.cachedTipXform = glm::translate(glm::mat4(1.f), end);
		pod.cachedTipXform = pod.cachedTipXform * glm::toMat4(tipRotation);
		pod.cachedTipXform = glm::scale(pod.cachedTipXform, glm::vec3(0.1f));
		pod.cachedNormalMatrix = glm::inverse(glm::transpose(pod.cachedTipXform));
		onValuesUpdated(pod);
	}

	VisualVector::~VisualVector()
	{
		numInstances--;
		if (numInstances == 0)
		{
			/*static*/ numInstances = 0;
			/*static*/ tipMesh = nullptr;
			/*static*/ tipMeshOffset = nullptr;
			/*static*/ tipShader = nullptr;
			/*static*/ lineRenderer = nullptr;
		}
	}

	TutorialEngine::VisualVector& VisualVector::operator=(const VisualVector& copy)
	{
		if (&copy != this)
		{
			pod = copy.pod;
			color = copy.color;

			numInstances++;
		}
		return *this;
	}

}
