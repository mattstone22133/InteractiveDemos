#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "EngineSystems/RenderSystem/Rendering/TextureWrapper.h"
#include "GameObjectBase.h"
#include "SceneNode.h"
#include "Transform.h"
#include "Utils/Platform/OpenGLES2/OpenGLES2Utils.h"
#include "Utils/Shader.h"
#include "Tools/RemoveSpecialMemberFunctionUtils.h"
#include "EngineSystems/RenderSystem/Rendering/GPUResource.h"


namespace Engine
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This code has largely been converted from some javascript classes I wrote. I only mention this to help
	// you understand why some of the classes and data is structured the way it is.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////
	// shaders
	//////////////////////////////////////////////////////////////////////////////////////
	const char* const glyphShader_vs = R"(
    attribute vec4 vertPos;
    attribute vec2 texUVCoord;

    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;

    //-1.0 flips, 1.0 does not flip
    uniform float flipV;

    varying highp vec2 uvCoord;

    void main(){
        gl_Position = projection * view * model * vertPos;
        uvCoord = texUVCoord;
        uvCoord.y = uvCoord.y * flipV;
    }

	)";

	const char* const glyphShader_fs = R"(
    uniform sampler2D texture0;
    uniform highp vec3 color;

    varying highp vec2 uvCoord;

    void main(){
        gl_FragColor = texture2D(texture0, uvCoord);
        gl_FragColor = gl_FragColor * vec4(color, 1.0);

        if(gl_FragColor.a == 0.0) {
            discard;
        }
    }
	)";

	//////////////////////////////////////////////////////////////////////////////////////
	// vert data
	//////////////////////////////////////////////////////////////////////////////////////
	//these constants will occupy separate memory in each translation unit; perhas there is a better way to get these constants in a header-only restriction
	//quad indices
	// 2---3
	// | \ |
	// 0---1
	static const int quadIndices[] = { 0, 1, 2, 1, 3, 2 };
	static const float quad3DPositions_idx[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };
	static const float quad2DPositions_idx[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f };


	//////////////////////////////////////////////////////////////////////////////////////
	// Glyph classes
	//////////////////////////////////////////////////////////////////////////////////////

	class UVRenderBox
	{
	public:
		/** uv pos is bottom-left aligned */
		UVRenderBox(glm::vec2 uvPos, float width, float height)
		{
			pos = uvPos;
			width = width;
			height = height;
		}
	private:
		glm::vec2 pos;
		float width = 0.f;
		float height = 0.f;
	};

	class GlyphRenderer : public GPUResource
		, public IRemoveCopies
		, public IRemoveMoves
	{
		friend class BitmapTextblock3D;
	public:
		GlyphRenderer(const sp<Shader>& glyphShader,
			const sp<TextureWrapper>& fontTextureObj,
			glm::vec2 uvPos,
			float width, float height,
			float baselineOffsetY = 0.f)
		{
			this->glyphShader = glyphShader;
			this->fontTextureObj = fontTextureObj;
			this->uvPos = uvPos;
			this->width = width;
			this->height = height;
			this->baselineOffsetY = baselineOffsetY;
			this->color = glm::vec3(1, 1, 1);
		}
		virtual ~GlyphRenderer();

		virtual void onAcquireGPUResources() override;
		virtual void onReleaseGPUResources() override;

	public:
		void render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model);
		void setColor(glm::vec3 inColor) { this->color = inColor; }
		glm::vec3 getColor() { return this->color; }
		void setKey(char inKey) { this->key = inKey; }

	private:
		void _createBuffers();
		void tryReadAttributesLocations();

		void preparePositionVBO();
		void prepareUVVBO();
		void bindEBO();

	private:
		sp<Shader> glyphShader = nullptr;
		sp<TextureWrapper> fontTextureObj = nullptr;
		glm::vec2 uvPos{ 0.f };
		float width = 1.0f;
		float height = 1.0f;
		float baselineOffsetY = 0.f;
		glm::vec3 color{ 1, 1, 1 };
		/*GLuint*/ uint32_t posVBO = 0;
		/*GLuint*/ uint32_t uvVBO = 0;
		/*GLuint*/ uint32_t ebo = 0;
		/*GLuint*/ uint32_t vao = 0;
		uint32_t attribLoc_pos = 0;
		uint32_t attribLoc_uv = 1;
		const char* posAttribName = "vertPos";
		const char* uvAttribName = "texUVCoord";
		char key = 0;
		bool bHasValidAttributeLocations = false;

		//uniform locations
		/*GLint*/ int32_t projection_ul;
		/*GLint*/ int32_t view_ul;
		/*GLint*/ int32_t model_ul;
		/*GLint*/ int32_t flipV_ul;
		/*GLint*/ int32_t color_ul;
		/*GLint*/ int32_t textureSampler_ul;
	};

	/**
	 * Notes:
	 *  to make the math easy, font textures are expected to have a size that is a square power of 2; eg 1024 x 1024. Otherwise
	 *     there will be some stretching that will need to be accounted for.
	 */
	class BitmapFont
	{
	public:
		BitmapFont(const BitmapFont& copy) = delete;
		BitmapFont(BitmapFont&& move) = delete;
		BitmapFont& operator=(const BitmapFont& copy) = delete;
		BitmapFont& operator=(BitmapFont&& move) = delete;
		BitmapFont(const char* exe_relative_texture_file_path)
		{
			Shader::ShaderParams shaderInit;
			shaderInit.vertex_src = glyphShader_vs;
			shaderInit.fragment_src = glyphShader_fs;

			shader = new_sp<Shader>(shaderInit);

			TextureWrapper::TextureInitParams texInit;
			texInit.exe_relative_filepath = exe_relative_texture_file_path;
			texInit.bGenerateMips = false;

			fontTexture = new_sp<TextureWrapper>(texInit);
			defaultGlyph = new_sp<GlyphRenderer>(shader, fontTexture, glm::vec2(0.f, 0.8f), 0.1f, 0.1f); //perhaps should not show anything? Will probably be more useful when debugging to see something
			_createLookupHashTable();
		}

		const sp<Shader>& getGlyphShader()
		{
			return shader;
		}

		const sp<GlyphRenderer> getGlyphFor(char letter)
		{
			//TODO this should probably return a glyph instance, rather than the actual GlyphRenderer
			const sp<GlyphRenderer>& glyph = glyphTable[letter];
			if (glyph == nullptr)
			{
				return defaultGlyph;
			}
			return glyph;
		}

		void setFontColor(glm::vec3 newColor = glm::vec3(1, 1, 1))
		{
			for (auto kv_pair : glyphTable)
			{
				const sp<GlyphRenderer>& glyph = kv_pair.second;
				if (glyph)
				{
					glyph->setColor(newColor);
				}
			}
		}

		glm::vec3 getFontColor()
		{
			if (const sp<GlyphRenderer>& glyph = glyphTable['a'])
			{
				return glyph->getColor();
			}
			return glm::vec3(1.f);
		}

	private:
		void _createLookupHashTable()
		{
			//I prefer to create this table upfront with null values, then have subclasses overwrite values.
			//that way, the structure of what this should look like is defined in the base class
			//NOTE: implementing this as a hashtable like map is probably inherently slower than using an array with index structure (where symbol is index)
			glyphTable['a'] = sp<GlyphRenderer>(nullptr);
			glyphTable['b'] = sp<GlyphRenderer>(nullptr);
			glyphTable['c'] = sp<GlyphRenderer>(nullptr);
			glyphTable['d'] = sp<GlyphRenderer>(nullptr);
			glyphTable['e'] = sp<GlyphRenderer>(nullptr);
			glyphTable['f'] = sp<GlyphRenderer>(nullptr);
			glyphTable['g'] = sp<GlyphRenderer>(nullptr);
			glyphTable['h'] = sp<GlyphRenderer>(nullptr);
			glyphTable['i'] = sp<GlyphRenderer>(nullptr);
			glyphTable['j'] = sp<GlyphRenderer>(nullptr);
			glyphTable['k'] = sp<GlyphRenderer>(nullptr);
			glyphTable['l'] = sp<GlyphRenderer>(nullptr);
			glyphTable['m'] = sp<GlyphRenderer>(nullptr);
			glyphTable['n'] = sp<GlyphRenderer>(nullptr);
			glyphTable['o'] = sp<GlyphRenderer>(nullptr);
			glyphTable['p'] = sp<GlyphRenderer>(nullptr);
			glyphTable['q'] = sp<GlyphRenderer>(nullptr);
			glyphTable['r'] = sp<GlyphRenderer>(nullptr);
			glyphTable['s'] = sp<GlyphRenderer>(nullptr);
			glyphTable['t'] = sp<GlyphRenderer>(nullptr);
			glyphTable['u'] = sp<GlyphRenderer>(nullptr);
			glyphTable['v'] = sp<GlyphRenderer>(nullptr);
			glyphTable['w'] = sp<GlyphRenderer>(nullptr);
			glyphTable['x'] = sp<GlyphRenderer>(nullptr);
			glyphTable['y'] = sp<GlyphRenderer>(nullptr);
			glyphTable['z'] = sp<GlyphRenderer>(nullptr);

			glyphTable['A'] = sp<GlyphRenderer>(nullptr);
			glyphTable['B'] = sp<GlyphRenderer>(nullptr);
			glyphTable['C'] = sp<GlyphRenderer>(nullptr);
			glyphTable['D'] = sp<GlyphRenderer>(nullptr);
			glyphTable['E'] = sp<GlyphRenderer>(nullptr);
			glyphTable['F'] = sp<GlyphRenderer>(nullptr);
			glyphTable['G'] = sp<GlyphRenderer>(nullptr);
			glyphTable['H'] = sp<GlyphRenderer>(nullptr);
			glyphTable['I'] = sp<GlyphRenderer>(nullptr);
			glyphTable['J'] = sp<GlyphRenderer>(nullptr);
			glyphTable['K'] = sp<GlyphRenderer>(nullptr);
			glyphTable['L'] = sp<GlyphRenderer>(nullptr);
			glyphTable['M'] = sp<GlyphRenderer>(nullptr);
			glyphTable['N'] = sp<GlyphRenderer>(nullptr);
			glyphTable['O'] = sp<GlyphRenderer>(nullptr);
			glyphTable['P'] = sp<GlyphRenderer>(nullptr);
			glyphTable['Q'] = sp<GlyphRenderer>(nullptr);
			glyphTable['R'] = sp<GlyphRenderer>(nullptr);
			glyphTable['S'] = sp<GlyphRenderer>(nullptr);
			glyphTable['T'] = sp<GlyphRenderer>(nullptr);
			glyphTable['U'] = sp<GlyphRenderer>(nullptr);
			glyphTable['V'] = sp<GlyphRenderer>(nullptr);
			glyphTable['W'] = sp<GlyphRenderer>(nullptr);
			glyphTable['X'] = sp<GlyphRenderer>(nullptr);
			glyphTable['Y'] = sp<GlyphRenderer>(nullptr);
			glyphTable['Z'] = sp<GlyphRenderer>(nullptr);

			//numeric row
			glyphTable['0'] = sp<GlyphRenderer>(nullptr);
			glyphTable['1'] = sp<GlyphRenderer>(nullptr);
			glyphTable['2'] = sp<GlyphRenderer>(nullptr);
			glyphTable['3'] = sp<GlyphRenderer>(nullptr);
			glyphTable['4'] = sp<GlyphRenderer>(nullptr);
			glyphTable['5'] = sp<GlyphRenderer>(nullptr);
			glyphTable['6'] = sp<GlyphRenderer>(nullptr);
			glyphTable['7'] = sp<GlyphRenderer>(nullptr);
			glyphTable['8'] = sp<GlyphRenderer>(nullptr);
			glyphTable['9'] = sp<GlyphRenderer>(nullptr);
			glyphTable['-'] = sp<GlyphRenderer>(nullptr);
			glyphTable['='] = sp<GlyphRenderer>(nullptr);

			//numeric row + shift
			glyphTable['!'] = sp<GlyphRenderer>(nullptr);
			glyphTable['@'] = sp<GlyphRenderer>(nullptr);
			glyphTable['#'] = sp<GlyphRenderer>(nullptr);
			glyphTable['$'] = sp<GlyphRenderer>(nullptr);
			glyphTable['%'] = sp<GlyphRenderer>(nullptr);
			glyphTable['^'] = sp<GlyphRenderer>(nullptr);
			glyphTable['&'] = sp<GlyphRenderer>(nullptr);
			glyphTable['*'] = sp<GlyphRenderer>(nullptr);
			glyphTable['('] = sp<GlyphRenderer>(nullptr);
			glyphTable[')'] = sp<GlyphRenderer>(nullptr);
			glyphTable['_'] = sp<GlyphRenderer>(nullptr);
			glyphTable['+'] = sp<GlyphRenderer>(nullptr);

			//symbols within keyboard letters
			glyphTable[';'] = sp<GlyphRenderer>(nullptr);
			glyphTable[':'] = sp<GlyphRenderer>(nullptr);
			glyphTable['\''] = sp<GlyphRenderer>(nullptr);
			glyphTable['\\'] = sp<GlyphRenderer>(nullptr);
			glyphTable['['] = sp<GlyphRenderer>(nullptr);
			glyphTable['{'] = sp<GlyphRenderer>(nullptr);
			glyphTable[']'] = sp<GlyphRenderer>(nullptr);
			glyphTable['}'] = sp<GlyphRenderer>(nullptr);
			glyphTable['/'] = sp<GlyphRenderer>(nullptr);
			glyphTable['?'] = sp<GlyphRenderer>(nullptr);
			glyphTable['.'] = sp<GlyphRenderer>(nullptr);
			glyphTable['>'] = sp<GlyphRenderer>(nullptr);
			glyphTable[','] = sp<GlyphRenderer>(nullptr);
			glyphTable['<'] = sp<GlyphRenderer>(nullptr);
			glyphTable['\\'] = sp<GlyphRenderer>(nullptr);
			glyphTable['|'] = sp<GlyphRenderer>(nullptr);
			glyphTable['`'] = sp<GlyphRenderer>(nullptr); //backtick (beside 1)
			glyphTable['~'] = sp<GlyphRenderer>(nullptr);

			//mathematical symbols
			//glyphTable['÷'] = sp<GlyphRenderer>(nullptr); //warning: illegal character encoding in character literal [-Winvalid-source-encoding]

			//symbols
			//glyphTable['©'} = sp<GlyphRenderer>(nullptr); 
			//glyphTable['®'} = sp<GlyphRenderer>(nullptr);

			//accents
			//glyphTable['ç'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['â'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['à'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['é'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['è'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['ê'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['ë'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['î'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['ï'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['ô'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['û'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['ù'] = sp<GlyphRenderer>(nullptr);
			//glyphTable['ü'] = sp<GlyphRenderer>(nullptr);
			//there exists more accent than these
		}

	protected:
		std::unordered_map<char, sp<GlyphRenderer>> glyphTable;
		sp<Shader> shader = nullptr;
		sp<TextureWrapper> fontTexture = nullptr;
		sp<GlyphRenderer>defaultGlyph = nullptr;
	};

	/** A rendering utility for rendering a glyph */
	//class GlyphInstance
	//{
	//	constructor(glyphData)
	//	{
	//		glyphData = glyphData;
	//	}
	//};

	enum class VAlignment : uint8_t
	{
		TOP,
		CENTER,
		BOTTOM,
	};
	enum class HAlignment : uint8_t
	{
		LEFT,
		CENTER,
		RIGHT
	};

	class BitmapTextblock3D
	{
	public:
		BitmapTextblock3D(
			const sp<BitmapFont>& inBitMapFont,
			const std::string& startText = "",
			float x = 0.f, float y = 0.f, float z = 0.f)
		{
			this->bitMapFont = inBitMapFont;
			this->text = startText;
			this->xform = Transform{};
			this->xform.position = glm::vec3(x, y, z);
			this->xform.scale = glm::vec3(1, 1, 1);
			this->parentModelMat = glm::mat4(1.f);
			this->hAlignment = HAlignment::RIGHT;
			this->vAlignment = VAlignment::BOTTOM;
			this->localWidth = 0;

			//setup
			calculateLocalWidth();
		}

		//call this if tweaking any values regarding the font; this is the function resolve any "dirty" state about the bitmap font.
		void refreshState()
		{
			calculateLocalWidth();
		}

		void render(glm::mat4 projection, glm::mat4 view)
		{
			//this isn't the fastest text renderer as it renders each glyph separating rather than
			//caching them within a texture and rendering that texture each time.
			if (text.length() > 0)
			{
				float width_so_far = 0;

				//calculate width for pivot matrix
				for (size_t char_idx = 0; char_idx < text.length(); ++char_idx)
				{
					const sp<GlyphRenderer> glyph = bitMapFont->getGlyphFor(text[char_idx]);
					width_so_far += glyph->width;
				}

				const sp<GlyphRenderer>& sizeReferenceGlyph = bitMapFont->getGlyphFor('A');

				glm::mat4 pivotMatrix{ 1.f };
				glm::vec3 pivotPos(0, 0, 0);

				//BEFORE CHANING PIVOT ALIGNMENT: right alight means the cursor appears on the right; left align means
				//the cursor is on the left. Top align means an imaginary cursor would be on top. So, a left algined
				//text will actually have a pivot point on the right (think about: the text grows towards the right; where's the stationary point?)
				float hAlignFactor = 0.f;
				if (hAlignment == HAlignment::LEFT) { hAlignFactor = -1.f; }
				else if (hAlignment == HAlignment::CENTER) { hAlignFactor = -0.5; } //move by half length

				float vAlignFactor = 0.f;
				if (vAlignment == VAlignment::TOP) { vAlignFactor = -1.f; }
				else if (vAlignment == VAlignment::CENTER) { vAlignFactor = -0.5; } //move by half length
				pivotPos.x = width_so_far * hAlignFactor;
				pivotPos.y = sizeReferenceGlyph->height * vAlignFactor;
				pivotMatrix = glm::translate(pivotMatrix, pivotPos);

				glm::mat4 sceneModelMat{ 1.f };
				if (parentModelMat.has_value())
				{
					sceneModelMat = (*parentModelMat) * sceneModelMat;
				}
				glm::mat4 textBlockModelMat = xform.getModelMatrix();
				sceneModelMat = sceneModelMat * textBlockModelMat;

				//transform bitmap to parent space with pivot correction
				sceneModelMat = sceneModelMat * pivotMatrix;

				glm::vec3 glyphPos(0, 0, 0);
				float x_offset = 0;
				for (size_t char_idx = 0; char_idx < text.length(); ++char_idx)
				{
					const sp<GlyphRenderer>& glyph = bitMapFont->getGlyphFor(text[char_idx]);
					glyphPos.x = x_offset;
					glyphPos.y = glyph->baselineOffsetY;
					x_offset += glyph->width; //be sure add width after we've calculated start pos

					glm::mat4 glyphModelMat{ 1.f };
					glyphModelMat = glm::translate(glyphModelMat, glyphPos);

					//transform bitmap to parent space with pivot correction
					glyphModelMat = sceneModelMat * glyphModelMat;
					glyph->render(view, projection, glyphModelMat);
				}
			}
		}

		void calculateLocalWidth()
		{
			if (text.length() > 0)
			{
				float width_so_far = 0;

				//calculate width for pivot matrix
				for (size_t char_idx = 0; char_idx < text.length(); ++char_idx)
				{
					const sp<GlyphRenderer>& glyph = bitMapFont->getGlyphFor(text[char_idx]);
					width_so_far += glyph->width;
				}

				localWidth = width_so_far;
			}
			else
			{
				localWidth = 0;
			}

		}

		float getLocalWidth()
		{
			if (!localWidth)
			{
				calculateLocalWidth();
			}
			return localWidth;
		}
	public:
		std::optional<glm::mat4> parentModelMat = std::nullopt;
		sp<BitmapFont> bitMapFont;
		std::string text;
		Transform xform;
		HAlignment hAlignment = HAlignment::RIGHT;
		VAlignment vAlignment = VAlignment::BOTTOM;
		float localWidth = 0;
	};

	struct TextBlockSceneNode : public SceneNode
	{
	public:
		TextBlockSceneNode(const TextBlockSceneNode& copy) = delete;
		TextBlockSceneNode(TextBlockSceneNode&& move) = delete;
		TextBlockSceneNode& operator=(const TextBlockSceneNode& copy) = delete;
		TextBlockSceneNode& operator=(TextBlockSceneNode&& move) = delete;
		TextBlockSceneNode(sp<BitmapFont> font, const std::string& text)
			: SceneNode()
		{
			wrappedText = new_sp<BitmapTextblock3D>(font, text);
			wrappedText->hAlignment = HAlignment::CENTER;
			wrappedText->vAlignment = VAlignment::CENTER;

			v_CleanComplete();
		}

		virtual void v_CleanComplete() override
		{
			if (wrappedText)
			{
				wrappedText->parentModelMat = getWorldMat();
			}
		}

		void render(const glm::mat4& projection, const glm::mat4& view)
		{
			requestClean();
			wrappedText->render(projection, view);
		}
	public:
		sp<BitmapTextblock3D> wrappedText;
	};

	class RenderBox3D
	{
		struct Line
		{
			glm::vec3 pntA;
			glm::vec3 pntB;
		};

	public:
		RenderBox3D(const RenderBox3D& copy) = delete;
		RenderBox3D(RenderBox3D&& move) = delete;
		RenderBox3D& operator=(const RenderBox3D& copy) = delete;
		RenderBox3D& operator=(RenderBox3D&& move) = delete;
		RenderBox3D(glm::vec3 bottomLeftPnt, float width, float height)
		{
			pos = bottomLeftPnt;
			width = width;
			height = height;
			_calculatePoints();
		}

		std::vector<Line> toLines()
		{
			std::vector<Line> lines =
			{
				Line{glm::vec3{pos}, glm::vec3{_BR}}, //bottom line
				Line{glm::vec3{pos}, glm::vec3{_TL}}, //left line
				Line{glm::vec3{_TL}, glm::vec3{_TR}}, //top line
				Line{glm::vec3{_BR}, glm::vec3{_TR}} //right line
			};
			return lines;
		}
	private:
		void _calculatePoints()
		{
			_BR = glm::vec3(pos.x + width, pos.y, pos.z);
			_TR = glm::vec3(pos.x + width, pos.y + height, pos.z);
			_TL = glm::vec3(pos.x, pos.y + height, pos.z);
		}
	private:
		float width = 1.f;
		float height = 1.f;
		glm::vec3 pos{ 0.f };
		glm::vec3 _BR{ 0.f };
		glm::vec3 _TR{ 0.f };
		glm::vec3 _TL{ 0.f };
	};
}