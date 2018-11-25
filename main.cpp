
//internal includes
#include "common.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "drawcall.h"
#include "ImportSceneObject.h"

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <random>
#include <map>

static const GLsizei WIDTH = 1270, HEIGHT = 720; //размеры окна
static int filling = 0;
static bool keys[1024]; //массив состояний кнопок - нажата/не нажата
static GLfloat lastX = 400, lastY = 300; //исходное положение мыши
static bool firstMouse = true;
static bool g_captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
static bool g_capturedMouseJustNow = false;
static bool bloom = true; // Эффект bloom

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(float3(0.0f, 5.0f, 30.0f));

//функция для обработки нажатий на кнопки клавиатуры
void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//std::cout << key << std::endl;
	switch (key)
	{
	case GLFW_KEY_ESCAPE: //на Esc выходим из программы
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_SPACE: //на пробел переключение в каркасный режим и обратно
		if (action == GLFW_PRESS)
		{
			if (filling == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				filling = 1;
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				filling = 0;
			}
		}
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
  case GLFW_KEY_B:
    barelRoll();
    break;
  case GLFW_KEY_C:
    if (action == GLFW_PRESS)
      bloom = !bloom;
    break;
	default:
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

//функция для обработки клавиш мыши
void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    g_captureMouse = !g_captureMouse;


  if (g_captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    g_capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

//функция для обработки перемещения мыши
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = float(xpos);
    lastY = float(ypos);
    firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - lastX;
  GLfloat yoffset = lastY - float(ypos);  

  lastX = float(xpos);
  lastY = float(ypos);

  if (g_captureMouse)
    camera.ProcessMouseMove(xoffset, yoffset);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(GLfloat(yoffset));
}

void doCameraMovement(Camera &camera, GLfloat deltaTime)
{
  if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

void renderQuad(GLuint &quadVAO)
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
          -1.0f,  1.0f,  0.0f, 1.0f,
          -1.0f, -1.0f,  0.0f, 0.0f,
           1.0f, -1.0f,  1.0f, 0.0f,

          -1.0f,  1.0f,  0.0f, 1.0f,
           1.0f, -1.0f,  1.0f, 0.0f,
           1.0f,  1.0f,  1.0f, 1.0f
        };

        GLuint quadVBO;

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    glBindVertexArray(quadVAO);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

    glBindVertexArray(0);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void createSkyboxVAO(GLuint &skyboxVAO)
{
  float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    GLuint skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(0); GL_CHECK_ERRORS;
}

/*
\brief создать triangle strip плоскость и загрузить её в шейдерную программу
\param rows - число строк
\param cols - число столбцов
\param size - размер плоскости
\param vao - vertex array object, связанный с созданной плоскостью
*/
static int createTriStrip(int rows, int cols, float size, GLuint &vao)
{

  int numIndices = 2 * cols*(rows - 1) + rows - 1;

  std::vector<GLfloat> vertices_vec; //вектор атрибута координат вершин
  vertices_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> normals_vec; //вектор атрибута нормалей к вершинам
  normals_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> texcoords_vec; //вектор атрибут текстурных координат вершин
  texcoords_vec.reserve(rows * cols * 2);

  std::vector<float3> normals_vec_tmp(rows * cols, float3(0.0f, 0.0f, 0.0f)); //временный вектор нормалей, используемый для расчетов

  std::vector<int3> faces;         //вектор граней (треугольников), каждая грань - три индекса вершин, её составляющих; используется для удобства расчета нормалей
  faces.reserve(numIndices / 3);

  std::vector<GLuint> indices_vec; //вектор индексов вершин для передачи шейдерной программе
  indices_vec.reserve(numIndices);

  for (int z = 0; z < rows; ++z)
  {
    for (int x = 0; x < cols; ++x)
    {
      //вычисляем координаты каждой из вершин 
      float xx = -size / 2 + x*size / cols;
      float zz = -size / 2 + z*size / rows;
      
      vertices_vec.push_back(xx);
      vertices_vec.push_back(0);
      vertices_vec.push_back(zz);

      texcoords_vec.push_back(x / float(cols - 1)); // вычисляем первую текстурную координату u, для плоскости это просто относительное положение вершины
      texcoords_vec.push_back(z / float(rows - 1)); // аналогично вычисляем вторую текстурную координату v
    }
  }

  //primitive restart - специальный индекс, который обозначает конец строки из треугольников в triangle_strip
  //после этого индекса формирование треугольников из массива индексов начнется заново - будут взяты следующие 3 индекса для первого треугольника
  //и далее каждый последующий индекс будет добавлять один новый треугольник пока снова не встретится primitive restart index

  int primRestart = cols * rows;

  for (int x = 0; x < cols - 1; ++x)
  {
    for (int z = 0; z < rows - 1; ++z)
    {
      int offset = x*cols + z;

      //каждую итерацию добавляем по два треугольника, которые вместе формируют четырехугольник
      if (z == 0) //если мы в начале строки треугольников, нам нужны первые четыре индекса
      {
        indices_vec.push_back(offset + 0);
        indices_vec.push_back(offset + rows);
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);
      }
      else // иначе нам достаточно двух индексов, чтобы добавить два треугольника
      {
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);

        if (z == rows - 2) indices_vec.push_back(primRestart); // если мы дошли до конца строки, вставляем primRestart, чтобы обозначить переход на следующую строку
      }
    }
  }

  //формируем вектор граней(треугольников) по 3 индекса на каждый
  int currFace = 1;
  for (int i = 0; i < indices_vec.size() - 2; ++i)
  {
    int3 face;

    int index0 = indices_vec.at(i);
    int index1 = indices_vec.at(i + 1);
    int index2 = indices_vec.at(i + 2);

    if (index0 != primRestart && index1 != primRestart && index2 != primRestart)
    {
      if (currFace % 2 != 0) //если это нечетный треугольник, то индексы и так в правильном порядке обхода - против часовой стрелки
      {
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 1);
        face.z = indices_vec.at(i + 2);

        currFace++;
      }
      else //если треугольник четный, то нужно поменять местами 2-й и 3-й индекс;
      {    //при отрисовке opengl делает это за нас, но при расчете нормалей нам нужно это сделать самостоятельно
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 2);
        face.z = indices_vec.at(i + 1);

        currFace++;
      }
      faces.push_back(face);
    }
  }

  //расчет нормалей
  for (int i = 0; i < faces.size(); ++i)
  {
    //получаем из вектора вершин координаты каждой из вершин одного треугольника
    float3 A(vertices_vec.at(3 * faces.at(i).x + 0), vertices_vec.at(3 * faces.at(i).x + 1), vertices_vec.at(3 * faces.at(i).x + 2));
    float3 B(vertices_vec.at(3 * faces.at(i).y + 0), vertices_vec.at(3 * faces.at(i).y + 1), vertices_vec.at(3 * faces.at(i).y + 2));
    float3 C(vertices_vec.at(3 * faces.at(i).z + 0), vertices_vec.at(3 * faces.at(i).z + 1), vertices_vec.at(3 * faces.at(i).z + 2));

    //получаем векторы для ребер треугольника из каждой из 3-х вершин
    float3 edge1A(normalize(B - A));
    float3 edge2A(normalize(C - A));

    float3 edge1B(normalize(A - B));
    float3 edge2B(normalize(C - B));

    float3 edge1C(normalize(A - C));
    float3 edge2C(normalize(B - C));

    //нормаль к треугольнику - векторное произведение любой пары векторов из одной вершины
    float3 face_normal = cross(edge1A, edge2A);

    //простой подход: нормаль к вершине = средняя по треугольникам, к которым принадлежит вершина
    normals_vec_tmp.at(faces.at(i).x) += face_normal;
    normals_vec_tmp.at(faces.at(i).y) += face_normal;
    normals_vec_tmp.at(faces.at(i).z) += face_normal;
  }

  //нормализуем векторы нормалей и записываем их в вектор из GLFloat, который будет передан в шейдерную программу
  for (int i = 0; i < normals_vec_tmp.size(); ++i)
  {
    float3 N = normalize(normals_vec_tmp.at(i));

    normals_vec.push_back(N.x);
    normals_vec.push_back(N.y);
    normals_vec.push_back(N.z);
  }


  GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboIndices);
  glGenBuffers(1, &vboNormals);
  glGenBuffers(1, &vboTexCoords);

  glBindVertexArray(vao); GL_CHECK_ERRORS;
  {

    //передаем в шейдерную программу атрибут координат вершин
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(GL_FLOAT), &vertices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут нормалей
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, normals_vec.size() * sizeof(GL_FLOAT), &normals_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(1); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут текстурных координат
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, texcoords_vec.size() * sizeof(GL_FLOAT), &texcoords_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

    //передаем в шейдерную программу индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_vec.size() * sizeof(GLuint), &indices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;

    glEnable(GL_PRIMITIVE_RESTART); GL_CHECK_ERRORS;
    glPrimitiveRestartIndex(primRestart); GL_CHECK_ERRORS;
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  return numIndices;
}


int initGL()
{
	int res = 0;

	//грузим функции opengl через glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	//выводим в консоль некоторую информацию о драйвере и контексте opengl
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Controls: "<< std::endl;
    std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
    std::cout << "press spacebar to alternate between shaded wireframe and fill display modes" << std::endl;
    std::cout << "press ESC to exit" << std::endl << std::endl;
    
	return 0;
}

enum Mode {
    DEBUG_TRIANGLE,
    SCENE
};

int main(int argc, char** argv) {
    Mode mode = DEBUG_TRIANGLE;
    GLFWwindow*  window = nullptr;

    if (argc >= 2) {
        mode = string(argv[1]) == "-planescene" ? SCENE : DEBUG_TRIANGLE;
    }

    try {
    
    if(!glfwInit())
    return -1;

	//запрашиваем контекст opengl версии 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
	
    if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	//регистрируем коллбеки для обработки сообщений от пользователя - клавиатура, мышь..
	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);
	glfwSetInputMode          (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

	if(initGL() != 0) 
		return -1;
	
    //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

    // Создание шейдерной программы из двух файлов с исходниками шейдеров
    // Используется класс-обертка ShaderProgram
    std::unordered_map<GLenum, std::string> shaders;
    shaders[GL_VERTEX_SHADER] = "shaders/vertex.glsl";
    shaders[GL_FRAGMENT_SHADER] = "shaders/fragment.glsl";
    ShaderProgram program(shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> terrain_shaders;
    terrain_shaders[GL_VERTEX_SHADER] = "shaders/terrain_vertex.glsl";
    terrain_shaders[GL_FRAGMENT_SHADER] = "shaders/terrain_fragment.glsl";
    ShaderProgram terrain_program(terrain_shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> debug_shaders;
    debug_shaders[GL_VERTEX_SHADER] = "shaders/vertex_start.glsl";
    debug_shaders[GL_FRAGMENT_SHADER] = "shaders/fragment_start.glsl";
    ShaderProgram debug_program(debug_shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> rain_shaders;
    rain_shaders[GL_VERTEX_SHADER] = "shaders/rain_vertex.glsl";
    rain_shaders[GL_FRAGMENT_SHADER] = "shaders/rain_fragment.glsl";
    ShaderProgram rain_program(rain_shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> cloud_shaders;
    shaders[GL_VERTEX_SHADER] = "shaders/cloud_vertex.glsl";
    shaders[GL_FRAGMENT_SHADER] = "shaders/cloud_fragment.glsl";
    ShaderProgram cloud_program(shaders); GL_CHECK_ERRORS;

    std::unordered_map<GLenum, std::string> skybox_shaders;
    skybox_shaders[GL_VERTEX_SHADER] = "shaders/skybox_vertex.glsl";
    skybox_shaders[GL_FRAGMENT_SHADER] = "shaders/skybox_fragment.glsl";
    ShaderProgram skybox_program(skybox_shaders); GL_CHECK_ERRORS;
    
    std::unordered_map<GLenum, std::string> blur_shaders;
    blur_shaders[GL_VERTEX_SHADER] = "shaders/blur_vertex.glsl";
    blur_shaders[GL_FRAGMENT_SHADER] = "shaders/blur_fragment.glsl";
    ShaderProgram blur_program(blur_shaders); GL_CHECK_ERRORS;
              
    std::unordered_map<GLenum, std::string> bloom_shaders;
    bloom_shaders[GL_VERTEX_SHADER] = "shaders/bloom_vertex.glsl";
    bloom_shaders[GL_FRAGMENT_SHADER] = "shaders/bloom_fragment.glsl";
    ShaderProgram bloom_program(bloom_shaders); GL_CHECK_ERRORS;
    
    // Создаем кадровый буфер для постобработки
    unsigned int sceneFBO;
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO); GL_CHECK_ERRORS;
    
    unsigned int sceneColorBuf[2];
    glGenTextures(2, sceneColorBuf);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, sceneColorBuf[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, sceneColorBuf[i], 0);
    }

    unsigned int depthFBO;
    glGenRenderbuffers(1, &depthFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthFBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthFBO);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); GL_CHECK_ERRORS;

    // Создаем кадровый буфер для блюра
    unsigned int blurFBO[2];
    unsigned int blurColorBuf[2];
    glGenFramebuffers(2, blurFBO);
    glGenTextures(2, blurColorBuf);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i]);
        glBindTexture(GL_TEXTURE_2D, blurColorBuf[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColorBuf[i], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    

    // Создаем и загружаем геометрию поверхности
    GLuint vaoTriStrip;
    int triStripIndices = createTriStrip(300, 300, 2000, vaoTriStrip);

    vector<unique_ptr<Mesh>> scene;
    map<uint32_t, Material> materials;

    ImportSceneObjectFromFile("assets/plane.xml", scene, materials);

    // Создаем mesh для облаков и дождя
    auto cloud_mesh = CreateCloudMesh();
    auto rain_mesh = CreateRaindropMesh();

    // Загружаем изображения для skybox
    vector<std::string> faces
    {
        "assets/skybox/right.jpg",
        "assets/skybox/left.jpg",
        "assets/skybox/top.jpg",
        "assets/skybox/bottom.jpg",
        "assets/skybox/front.jpg",
        "assets/skybox/back.jpg",
    };

    unsigned int cubemapTexture = loadCubemap(faces);

    GLuint skyboxVAO;
    createSkyboxVAO(skyboxVAO);

    glEnable(GL_DEPTH_TEST); GL_CHECK_ERRORS;

	// Цикл обработки сообщений и отрисовки сцены каждый кадр
	while (!glfwWindowShouldClose(window))
	{
		// Считаем сколько времени прошло за кадр
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
        doCameraMovement(camera, deltaTime);

        switch (mode) {
        case SCENE: {
            
            // Отрисовка сцены
            glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO); GL_CHECK_ERRORS;

            glClearColor(0.1f, 0.6f, 0.8f, 1.0f); GL_CHECK_ERRORS;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

            float4x4 view = camera.GetViewMatrix();
            float4x4 projection = projectionMatrixTransposed(camera.zoom, float(WIDTH) / float(HEIGHT), 0.1f, 1000.0f);

            skybox_program.StartUseShader();
            glDepthMask(GL_FALSE);

            skybox_program.SetUniform("view", view);
            skybox_program.SetUniform("projection", projection);
            skybox_program.SetUniform("skybox", 5);

            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glDepthMask(GL_TRUE);
            skybox_program.StopUseShader();
            
            for (auto &mesh : scene) {
                program.StartUseShader(); GL_CHECK_ERRORS;
                
                // Матераил без текстуры содержит -1 в поле tex_id
                if (materials[mesh->material_id].tex_id != -1) {
                    glActiveTexture(GL_TEXTURE2); GL_CHECK_ERRORS;
                    glBindTexture(GL_TEXTURE_2D, materials[mesh->material_id].tex_id); GL_CHECK_ERRORS;
                }

                DrawMesh(program, camera, mesh, WIDTH, HEIGHT, deltaTime);

                program.StopUseShader(); GL_CHECK_ERRORS;
            }

            float4x4 model = transpose(translate4x4(float3(0, -60, 0)));
        
            terrain_program.StartUseShader();
        
            static float2 plane_pos = float2(0, 0);
            static float2 speed = float2(10, 0);

            plane_pos.x += deltaTime * speed.x;
            plane_pos.y += deltaTime * speed.y;

            terrain_program.SetUniform("view", view); GL_CHECK_ERRORS;
            terrain_program.SetUniform("projection", projection); GL_CHECK_ERRORS;
            terrain_program.SetUniform("model", model); GL_CHECK_ERRORS;
        
            terrain_program.SetUniform("plane_pos", plane_pos); GL_CHECK_ERRORS;

            glBindVertexArray(vaoTriStrip);
            glDrawElements(GL_TRIANGLE_STRIP, triStripIndices, GL_UNSIGNED_INT, 0);

            terrain_program.StopUseShader();

            DrawClouds(cloud_program, camera, cloud_mesh, WIDTH, HEIGHT, deltaTime);
            DrawRain(rain_program, camera, rain_mesh, WIDTH, HEIGHT, deltaTime);
            
            // Постобработка
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glClearColor(0.1f, 0.6f, 0.8f, 1.0f); GL_CHECK_ERRORS;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
            
            bool horizontal = true, firstIter = true;
            unsigned int blurRadius = 10;
            GLuint quadVAO = 0;

            blur_program.StartUseShader();

            blur_program.SetUniform("image", 7); GL_CHECK_ERRORS;

            for (unsigned int i = 0; i < blurRadius; i++) {
                glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[horizontal]);

                blur_program.SetUniform("horizontal", horizontal);

                glActiveTexture(GL_TEXTURE7);
                glBindTexture(GL_TEXTURE_2D, firstIter ? sceneColorBuf[1] : blurColorBuf[!horizontal]);

                renderQuad(quadVAO);

                horizontal = !horizontal;
                if (firstIter)
                    firstIter = false;
            }

            blur_program.StopUseShader();
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            bloom_program.StartUseShader();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sceneColorBuf[0]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, blurColorBuf[!horizontal]);

            bloom_program.SetUniform("bloom", bloom); GL_CHECK_ERRORS;
            bloom_program.SetUniform("scene", 0); GL_CHECK_ERRORS;
            bloom_program.SetUniform("bloomBlur", 1); GL_CHECK_ERRORS;

            renderQuad(quadVAO);

            bloom_program.StopUseShader();

        } break;
        case DEBUG_TRIANGLE: {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
            
            DrawSimpleTriangle(debug_program, camera, WIDTH, HEIGHT);
        } break;
        };

        glfwSwapBuffers(window); 
	}

	// Очищаем vao перед закрытием программы
	glDeleteVertexArrays(1, &vaoTriStrip);

	glfwTerminate();
    }
    catch (std::exception &e) {
        if (window) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        glfwTerminate();
        std::cout << std::endl << "Exception happened! See details." << std::endl;
        std::cout << e.what() << std::endl;

        std::cout << std::endl << "Press any button to continue..." << std::endl;
        std::cin.get();
    }

    return 0;
}
