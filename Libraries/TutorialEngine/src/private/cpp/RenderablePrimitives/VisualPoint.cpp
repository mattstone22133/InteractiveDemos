#include "RenderablePrimitives/VisualPoint.h"

#include "Utils/Shader.h"
#include "RenderablePrimitives/LineRenderer.h"
#include "Utils/MathUtils.h"
#include "GameObjectBase.h"
#include "Geometry/SphereMesh_GpuBuffer.h"


namespace TutorialEngine
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// statics
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*static*/int TutorialEngine::VisualPoint::numInstances = 0;
	/*static*/sp<Engine::SphereMesh_GpuBuffer> VisualPoint::pointMesh = nullptr;
	/*static*/sp<Engine::Shader> VisualPoint::pointShader = nullptr;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// members
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VisualPoint::VisualPoint()
	{
		numInstances++;
		if (numInstances == 1)
		{
			Engine::Shader::ShaderParams sphereShaderInit;

			sphereShaderInit.vertex_src = R"(
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
			sphereShaderInit.fragment_src = R"(
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
			pointShader = new_up<Engine::Shader>(sphereShaderInit);
			//pointMesh = new_sp<StaticMesh::Model>("./assets/models/sphere/ico_sphere.obj");
			pointMesh = new_sp<Engine::SphereMesh_GpuBuffer>(); //#todo #static_mesh_refactor need to specify radius to keep parity 
		}
	}

	VisualPoint::VisualPoint(const VisualPoint& copy)
	{
		if (&copy != this)
		{
			pod = copy.pod;
			color = copy.color;

			numInstances++;
		}
	}


	VisualPoint::VisualPoint(VisualPoint&& move)
	{
		if (&move != this)
		{
			color = move.color;
			pod = move.pod;

			//need to increment because moving argument will be haves it dtor called
			numInstances++;
		}
	}

	TutorialEngine::VisualPoint& VisualPoint::operator=(VisualPoint&& move)
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

	void VisualPoint::render(const glm::mat4& projection_view, std::optional<glm::vec3> cameraPos) const
	{
		if (pointShader 
			&& pointShader->hasAcquiredResources() //make sure shader can have uniform values set
			)
		{
			pointShader->use();
			pointShader->setMat4("model", pod.cachedXform);
			pointShader->setMat4("projection_view", projection_view);
			pointShader->setUniform3f("solidColor", color);

			if (cameraPos)
			{
				pointShader->setUniform3f("cameraPos", *cameraPos);
				pointShader->setUniform1i("bUseCameraLight", true);
				pointShader->setUniform1i("bUseLight", true);
			}
			else
			{
				pointShader->setUniform1i("bUseCameraLight", true);
				pointShader->setUniform1i("bUseLight", false);
			}

			pointMesh->render(*pointShader, /*activatShader*/false);
		}
	}

	void VisualPoint::setPosition(glm::vec3 newStart)
	{
		pod.position = newStart;

		updateCache();
	}

	void VisualPoint::setUserScale(glm::vec3 newScale)
	{
		pod.scale = newScale;
		updateCache();
	}

	void VisualPoint::updateCache()
	{
		pod.cachedXform = glm::translate(glm::mat4(1.f), pod.position);
		pod.cachedXform = glm::scale(pod.cachedXform, glm::vec3(0.025f)); //always scale down to give intuitive feel for scale 1.0f
		pod.cachedXform = glm::scale(pod.cachedXform, pod.scale);
		onValuesUpdated(pod);
		eventValuesUpdated.broadcast(*this);
	}

	VisualPoint::~VisualPoint()
	{
		numInstances--;
		if (numInstances == 0)
		{
			/*static*/ numInstances = 0;
			/*static*/ pointMesh = nullptr;
			/*static*/ pointShader = nullptr;
		}
	}

	TutorialEngine::VisualPoint& VisualPoint::operator=(const VisualPoint& copy)
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
