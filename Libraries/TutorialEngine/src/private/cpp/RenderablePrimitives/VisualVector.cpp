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

					uniform mat4 model = mat4(1.0f);
					uniform mat4 projection_view = mat4(1.0f);

					void main(){
						worldPos = (model * vec4(position,1.f)).xyz;
						worldNormal = normalize((inverse(transpose(model)) * vec4(normal, 0.f)).xyz);
						fragUV = uv;

						gl_Position = projection_view * vec4(worldPos, 1.f);
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

					uniform vec3 dirLight = normalize(vec3(-1,-1,-1));
					uniform vec3 solidColor = vec3(1,0,0);
					uniform bool bUseLight = false;
					uniform bool bUseTexture = false;
					uniform bool bUseCameraLight = false;
					uniform vec3 cameraPos = vec3(0,0,0);

					struct Material
					{
						sampler2D texture_diffuse0;
					};
					uniform Material material;					

					void main()
					{
						vec4 diffuseTexture = texture(material.texture_diffuse0, fragUV); //wasteful as we may not use this, but since this is demo leaving as it

						if(bUseLight)
						{
							vec3 toLight_n = bUseCameraLight ? normalize(cameraPos - worldPos) : normalize(dirLight);
							float diffuseFactor = max(dot(toLight_n, worldNormal.xyz),0);

							vec3 colorToUse = bUseTexture ? diffuseTexture.xyz : solidColor.xyz;
							vec3 diffuse = colorToUse * diffuseFactor;
							vec3 ambient = colorToUse * 0.05;
							gl_FragColor = vec4(ambient+diffuse, 1.0f);
						}
						else
						{
							if(bUseTexture)
							{
								gl_FragColor = vec4(diffuseTexture.rgb, 1.0f);
							}
							else
							{
								gl_FragColor = vec4(solidColor.rgb, 1.0f);
							}
						}
					}
				)";
			tipShader = new_up<Engine::Shader>(coneShaderInit);
			lineRenderer = new_sp<TutorialEngine::LineRenderer>();
			//tipMesh = new_sp<StaticMesh::Model>("./assets/models/cone_tip/cone_tip_centered.obj"); 
			//tipMeshOffset = new_sp<StaticMesh::Model>("./assets/models/cone_tip/cone_tip.obj");
			tipMesh = new_sp<ConeMesh_GpuBuffer>(/*numFaces*/5, /*height*/1.0f, /*radius*/ 1.0f,ConeGenerator::ConeOriginGeneration::BASE);
			tipMeshOffset = new_sp<ConeMesh_GpuBuffer>(/*numFaces*/5, /*height*/1.0f, /*radius*/ 1.0f, ConeGenerator::ConeOriginGeneration::TIP);
			//#todo #static_mesh_refactor verify above looks correct, refactoring away from using stati cmeshes but cannot compile code atm to verify. likely requires iterator.
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


	VisualVector::VisualVector(VisualVector&& move)
	{
		if (&move != this)
		{
			color = move.color;
			pod = move.pod;

			//need to increment because moving argument will be haves it dtor called
			numInstances++;
		}
	}

	TutorialEngine::VisualVector& VisualVector::operator=(VisualVector&& move)
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
