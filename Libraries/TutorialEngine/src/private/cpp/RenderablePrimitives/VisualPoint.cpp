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

					uniform highp mat4 model;// = mat4(1.0);
					uniform highp mat4 normal_matrix;// = mat4(1.0);
					uniform highp mat4 projection_view;// = mat4(1.0);

					void main(){
						worldPos = (model * vec4(position,1.0)).xyz;
						//worldNormal = normalize( (inverse(transpose(model)) * vec4(normal, 0.0)).xyz);  //inverse not presentin gles2
						worldNormal = normalize( normal_matrix * vec4(normal, 0.0)).xyz;
						fragUV = uv;

						gl_Position = projection_view * vec4(worldPos, 1.0);
					}
				)";
			sphereShaderInit.fragment_src = R"(
					varying highp vec3 worldPos;
					varying highp vec3 worldNormal;
					varying highp vec2 fragUV;

					uniform highp vec3 dirLight;		// = normalize(vec3(-1,-1,-1));
					uniform mediump vec3 solidColor;	// = vec3(1,0,0);
					uniform lowp int bUseLight;			// = false;
					uniform lowp int bUseCameraLight;	// = false;
					uniform highp vec3 cameraPos;		// = vec3(0,0,0);

					//NOTE: I removed a lot of texture usage during porting, if that is needed, perhaps best to go back to source and re-port this shader.
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
			sphereShaderInit.uniformInitializerFunc = [](class Engine::Shader& shader) 
			{
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
			pointShader = new_sp<Engine::Shader>(sphereShaderInit);
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

	void VisualPoint::onAcquireGPUResources()
	{
		//todo initialize shader variables on NEXT TICK
		//Engine::EngineBase::get().getGameTimeManager().createTimer()
	}

	void VisualPoint::onReleaseGPUResources()
	{
	}

	void VisualPoint::render(const glm::mat4& projection_view, std::optional<glm::vec3> cameraPos) const
	{
		if (pointShader 
			&& pointShader->hasAcquiredResources() //make sure shader can have uniform values set
			)
		{
			pointShader->use();
			pointShader->setMat4("model", pod.cachedXform);
			pointShader->setMat4("normal_matrix", pod.cachedNormalMatrix);
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
		pod.cachedNormalMatrix = glm::inverse(glm::transpose(pod.cachedXform));
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
