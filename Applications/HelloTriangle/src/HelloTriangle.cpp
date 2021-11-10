// see https://www.khronos.org/registry/OpenGL-Refpages/es2.0/ for es2 available opengl functions

#include<iostream>

// about using glfw library and emscripten
// https://stackoverflow.com/questions/27668931/using-libraries-with-emscripten

#include<GLFW/glfw3.h> //this must come before glad include, otherwise get macro redifintion issues; likely related to including windows.h; https://github.com/Dav1dde/glad/issues/283 glad2 apparently resolves this.
#ifdef HTML_BUILD //ONLY HTML
	#include <emscripten/emscripten.h>
	#include <GLES2/gl2.h>
#else //DESKTOP ONLY
	#include<glad/glad.h>
#endif

namespace HelloTriangle {

	int initialize();
	void mainLoopTick();
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);

	//config
	const GLsizei width = 800;
	const GLsizei height = 600;

	//application state
	GLFWwindow* window = nullptr;
	GLuint VBO = 0;	//will hold id of buffer object responsible for triangle
	GLuint shaderProgramID = 0;
	GLuint PositionAtributeLocation = 0;


	const char* vertextShader = R"(
attribute vec3 position;			
void main()
{
	gl_Position = vec4(position.x, position.y, position.z, 1.0);
}

	)";

	const char* fragmentShader = R"(
void main()
{
	gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}

	)";


	int initialize() {
		//set up glfw window management
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API); //WindowsOS will fail to create a window with this hint; it may be related to AMD cards not supporting gles2 via extensions like intel and nvidia does. https://www.saschawillems.de/blog/2015/04/19/using-opengl-es-on-windows-desktops-via-egl/
		glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API); //seems to be needed to get OpenGL ES context on WindowsOS.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); //https://www.glfw.org/docs/3.3.2/window_guide.html#GLFW_OPENGL_PROFILE_attrib says gles must use anyprofile;  GLFW_OPENGL_ANY_PROFILE if the OpenGL profile is unknown ****or the context is an OpenGL ES*** context.
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //If OpenGL ES is requested, this hint is ignored. https://www.glfw.org/docs/3.3/window_guide.html#GLFW_OPENGL_FORWARD_COMPAT_hint

		//create window
		window = glfwCreateWindow(width, height, "OpenGL Window", nullptr, nullptr);
		if (window == nullptr) {
			std::cout << "failed to create window" << std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);

#if !HTML_BUILD //GLAD is not used for emscripten, instead use "#include <GLES2/gl2.h>" with emscripten; these headers are added via emcmake 
		//check GLAD is working
		//if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress)) {
		if (!gladLoadGLES2Loader((GLADloadproc)&glfwGetProcAddress)) {
			std::cout << "failed to initialize glad" << std::endl;
			return -1;
		}
#endif
		//set up the view port dimensions
		glViewport(0, 0, width, height);

		//set up call back for resizing window
		glfwSetFramebufferSizeCallback(window, &framebuffer_size_callback);

		//-------------------------- SHADERS --------------------------------//
		//COMPILE VERTEX SHADER
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShaderID, 1, &vertextShader, nullptr); //second arg is number of strings you can pass it, 4th arg is an array of those string lengths
		glCompileShader(vertexShaderID);

		//test compilation of vertex shader 
		GLint vertexCompileSuccess = 0;
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &vertexCompileSuccess);
		if (!vertexCompileSuccess) {
			char log[512];
			glGetShaderInfoLog(vertexShaderID, 512, NULL, log);
			std::cout << "vertex shader compile fail" << log << std::endl;
		}

		//COMPILE FRAGMENT SHADER
		GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragShaderID, 1, &fragmentShader, nullptr);
		glCompileShader(fragShaderID);

		//check success of fragment shader compilation
		GLint fragShaderSuccess = 0;
		glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &fragShaderSuccess);
		if (!fragShaderSuccess) {
			char log[512];
			glGetShaderInfoLog(fragShaderID, 512, nullptr, log);
			std::cout << "frag shader failed to compile\n" << log << std::endl;
		}

		//LINK SHADERS INTO SHADER PROGRAM
		shaderProgramID = glCreateProgram();
		glAttachShader(shaderProgramID, vertexShaderID);
		glAttachShader(shaderProgramID, fragShaderID);
		glLinkProgram(shaderProgramID);

		//test linking of shader program
		GLint shaderLinkSuccess = 0;
		glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &shaderLinkSuccess);
		if (!shaderLinkSuccess) {
			char infoLog[512]; //not recycling logs for example readability
			glGetProgramInfoLog(shaderProgramID, 512, nullptr, infoLog);
			std::cout << "failed to link shaders in shader program\n" << infoLog << std::endl;
		}

		//shader program is ready to be used

		//delete shader objects since we've already linked them into the shader program
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragShaderID);

		// ------------------------- TRIANGLE DATA -------------------------//
		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
		};

		//VAOS NOT SUPPORTED IN GLES2
		//create a VAO (vertex array object) to store the vertex attributes's configuration state
		//GLuint VAO = 0;
		//glGenVertexArrays(1, &VAO);
		//glBindVertexArray(VAO); 

		//create VBO (vertex buffer object)
		glGenBuffers(1, &VBO);	//create 1 buffer, pass address of where id should be stored 

		//bind (connect) the type GL_ARRAY_BUFFER to the generated buffer at ID "VBO"
		glBindBuffer(GL_ARRAY_BUFFER, VBO); //operations on GL_ARRAY_BUFFER will now affect the VBO buffer object

		//buffer (load) the data into the buffer we created (GL_ARRAY_BUFFER is bound to VBO, which is how VBO gets data)
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// opengl es 2.0 doesn't have layout specifiers AFAIK, we need to query where it put the attribute so we can enable it later for rendering.
		PositionAtributeLocation = glGetAttribLocation(shaderProgramID, "position");

		//INSTRUCT OPENGL HOW TO INTERPRET PASSED DATA
		glVertexAttribPointer(PositionAtributeLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0)); // note: needs VBO already bound,
		glEnableVertexAttribArray(PositionAtributeLocation); //activates vertex attribute (shader variable) labeled with location of the attribute in the shader (gles2 does not have layout locations)
		
		//unbind the vertex array so other configuration changes are not stored in this VAO
		//glBindVertexArray(0); VAOs not supported in gles 2.0

		glUseProgram(shaderProgramID);//set the shader program to use 
		// -----------------------------------------------------------------//

		//render loop
#if HTML_BUILD
		emscripten_set_main_loop(&mainLoopTick, 60, true);
#else
		while (!glfwWindowShouldClose(window)) {
			mainLoopTick();
		}
#endif //HTML_BUILD

		return 0;
	}

	void mainLoopTick() {
		if(!glfwWindowShouldClose(window)) {
			//poll input
			processInput(window);

			//CLEAR SCREEN (render commands)
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			//SET SHADER FOR DRAWING
			glUseProgram(shaderProgramID);

			//DRAW TRIANGLE with configuration saved in vao
			//glBindVertexArray(VAO); //VAO's not supported in gles2
			glEnableVertexAttribArray(PositionAtributeLocation);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glVertexAttribPointer(PositionAtributeLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
			glDrawArrays(GL_TRIANGLES, 0, 3);

			//swap and put on screen
			glfwSwapBuffers(window);
			glfwPollEvents(); //update keyboard input, mouse movement, etc.
		}
		else
		{
			//deallocate resources
			//glDeleteVertexArrays(1, &VAO); //VAOs not supported in gles2
			glDeleteBuffers(1, &VBO);

			//free resources before closing
			glfwTerminate();
		}
	};

	void framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { //GLFW_RELEASE is the state where no key is pressed 
			glfwSetWindowShouldClose(window, true);
		}
	}

}

int main()
{
	HelloTriangle::initialize();
}
