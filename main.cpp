#include "stdafx.h"
#include "classes/system/Shader.h"
#include "classes/system/Scene.h"
#include "classes/system/FPSController.h"
#include "classes/buffers/StaticBuffer.h"

#define TT_LB 2
#define TT_RB 3
#define TT_RT 4
#define TT_LT 5
#define TT_B 6
#define TT_R 7
#define TT_T 8
#define TT_L 9
#define TT_HH 10
#define TT_VH 11

bool Pause;
bool keys[1024] = {0};
int WindowWidth = 800, WindowHeight = 600;
bool EnableVsync = 1;
GLFWwindow* window;
stFPSController FPSController;

MShader Shader;
MScene Scene;
MStaticBuffer StaticBuffer;

glm::vec2 TileSize(32, 32);
const int TilesCount[2] = {10, 10};
vector<NRectangle2> Rooms;

bool CreateWalls(vector<NRectangle2>* pRooms, int TilesCountX, int TilesCountY) {
	//edges of map must zero, otherwise some walls dissapear 
	if(!pRooms) return false;
	if(pRooms->empty()) return false;
	if(TilesCountX <= 0 || TilesCountY<=0) return false;
	
	unsigned char** Map;
	Map = new unsigned char* [TilesCountX];
	for(int i=0; i<TilesCountX; i++) {
		Map[i] = new unsigned char[TilesCountY];
		memset(Map[i], 0, sizeof(unsigned char) * TilesCountY);
	}
	
	//fill map
	for(int i=0; i<pRooms->size(); i++) {
		for(int j=pRooms->at(i).Position.y; j<pRooms->at(i).Position.y + pRooms->at(i).Size.y; j++) {
			for(int k=pRooms->at(i).Position.x; k<pRooms->at(i).Position.x + pRooms->at(i).Size.x; k++) {
				Map[j][k] = 1;
			}
		}
	}
	
	vector<NLine2> Lines;	
	NLine2 Line1, Line2;
	
	//get horizontal edges
	Line1.a.x = 0;
	Line2.a.x = 0;
	for(int i=0; i<TilesCount[1] - 1; i++) {
		for(int j=0; j<TilesCount[0]; j++) {
			if((!Map[i][j] && Map[i+1][j])) {
				if(!Line1.a.x ) {
					Line1.a.x = j;
					Line1.b.x = j;
					Line1.a.y = i+1;
					Line1.b.y = i+1;
				}
				else Line1.b.x ++;
			}
			else {
				if(Line1.a.x) {
					Line1.b.x ++;
					Lines.push_back(Line1);
					Line1.a.x = 0;
				}
			}
			if(Map[i][j] && !Map[i+1][j]) {
				if(!Line2.a.x ) {
					Line2.a.x = j;
					Line2.b.x = j;
					Line2.a.y = i+1;
					Line2.b.y = i+1;
				}
				else Line2.b.x ++;
			}
			else {
				if(Line2.a.x) {
					Line2.b.x ++;
					Lines.push_back(Line2);
					Line2.a.x = 0;
				}
			}
		}
	}
	
	//get vertical edges
	Line1.a.x = 0;
	Line2.a.x = 0;
	for(int i=0; i<TilesCount[0] - 1; i++) {
		for(int j=0; j<TilesCount[1]; j++) {
			if(!Map[j][i] && Map[j][i+1]) {
				if(!Line1.a.y) {
					Line1.a.y = j;
					Line1.b.y = j;
					Line1.a.x = i+1;
					Line1.b.x = i+1;
				}
				else Line1.b.y ++;
			}
			else {
				if(Line1.a.y) {
					Line1.b.y ++;
					Lines.push_back(Line1);
					Line1.a.y = 0;
				}
			}
			if(Map[j][i] && !Map[j][i+1]) {
				if(!Line2.a.y ) {
					Line2.a.y = j;
					Line2.b.y = j;
					Line2.a.x = i+1;
					Line2.b.x = i+1;
				}
				else Line2.b.y ++;
			}
			else {
				if(Line2.a.y) {
					Line2.b.y ++;
					Lines.push_back(Line2);
					Line2.a.y = 0;
				}
			}
		}
	}
	
	//add walls to visual
	StaticBuffer.Initialize();
	StaticBuffer.SetPrimitiveType(GL_LINES);
	for(int i=0; i<Lines.size(); i++) {
		if(Lines[i].a.x) {
			StaticBuffer.AddVertex(glm::vec2(Lines[i].a.x * TileSize.x, Lines[i].a.y * TileSize.y), glm::vec3(1, 1, 1));
			StaticBuffer.AddVertex(glm::vec2(Lines[i].b.x * TileSize.x, Lines[i].b.y * TileSize.y), glm::vec3(1, 1, 1));
		}
	}
	StaticBuffer.Dispose();
	
	//update map for future tiles textures
	for(int i=0; i<TilesCount[0] - 1; i++) {
		for(int j=0; j<TilesCount[1] - 1; j++) {
			//angles
			if(!Map[i][j] && !Map[i][j+1] && !Map[i+1][j] && Map[i+1][j+1]) Map[i+1][j+1] = TT_LT;
			if(!Map[i][j] && !Map[i][j+1] && Map[i+1][j] && !Map[i+1][j+1]) Map[i+1][j] = TT_RT;
			if(Map[i][j] && !Map[i][j+1] && !Map[i+1][j] && !Map[i+1][j+1]) Map[i][j] = TT_RB;
			if(!Map[i][j] && Map[i][j+1] && !Map[i+1][j] && !Map[i+1][j+1]) Map[i][j+1] = TT_LB;
			//1 size halls
			if(j > 0) {
				if(!Map[i][j-1] && Map[i][j] && !Map[i][j+1]) Map[i][j] = TT_HH;
			}
			if(i > 0) {
				if(!Map[i-1][j] && Map[i][j] && !Map[i+1][j]) Map[i][j] = TT_VH;
			}
			//walls
			if(!Map[i][j] && Map[i][j+1] == 1) Map[i][j+1] = TT_L;
			if(!Map[i][j] && Map[i+1][j] == 1) Map[i+1][j] = TT_T;
			if(Map[i][j] == 1 && !Map[i][j+1]) Map[i][j] = TT_R;
			if(Map[i][j] == 1 && !Map[i+1][j]) Map[i][j] = TT_B;
		}
	}
	
	for(int i=0; i<TilesCountX; i++) {
		for(int j=0; j<TilesCountY; j++) {
			printf("%3d", Map[i][j]);
		}
		cout<<endl;
	}
	
	for(int i=0; i<TilesCountX; i++) {
		if(Map[i]) delete [] Map[i];
	}
	if(Map) delete [] Map;
	
	return true;
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void mousepos_callback(GLFWwindow* window, double x, double y) {
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
}

bool InitApp() {
	LogFile<<"Starting application"<<endl;    
    glfwSetErrorCallback(error_callback);
    
    if(!glfwInit()) return false;
    window = glfwCreateWindow(WindowWidth, WindowHeight, "TestApp", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return false;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mousepos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);
    if(glfwExtensionSupported("WGL_EXT_swap_control")) {
    	LogFile<<"Window: V-Sync supported. V-Sync: "<<EnableVsync<<endl;
		glfwSwapInterval(EnableVsync);//0 - disable, 1 - enable
	}
	else LogFile<<"Window: V-Sync not supported"<<endl;
    LogFile<<"Window created: width: "<<WindowWidth<<" height: "<<WindowHeight<<endl;

	//glew
	GLenum Error = glewInit();
	if(GLEW_OK != Error) {
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return false;
	}
	LogFile<<"GLEW initialized"<<endl;
	
	if(!CheckOpenglSupport()) return false;

	//shaders
	if(!Shader.CreateShaderProgram("shaders/main.vertexshader.glsl", "shaders/main.fragmentshader.glsl")) return false;
	if(!Shader.AddUnifrom("MVP", "MVP")) return false;
	LogFile<<"Shaders loaded"<<endl;

	//scene
	if(!Scene.Initialize(&WindowWidth, &WindowHeight)) return false;
	LogFile<<"Scene initialized"<<endl;

	//randomize
    srand(time(NULL));
    LogFile<<"Randomized"<<endl;
    
    //other initializations
    Rooms.push_back(NRectangle2(NVector2(1, 1), NVector2(3, 3)));
    Rooms.push_back(NRectangle2(NVector2(1, 6), NVector2(3, 3)));
    Rooms.push_back(NRectangle2(NVector2(5, 1), NVector2(4, 4)));
    Rooms.push_back(NRectangle2(NVector2(2, 4), NVector2(1, 2)));
    Rooms.push_back(NRectangle2(NVector2(4, 8), NVector2(2, 1)));
    Rooms.push_back(NRectangle2(NVector2(6, 5), NVector2(1, 4)));
    Rooms.push_back(NRectangle2(NVector2(4, 2), NVector2(1, 1)));
    CreateWalls(&Rooms, 10, 10);
	
	//turn off pause
	Pause = false;
    
    return true;
}

void RenderStep() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(Shader.ProgramId);
	glUniformMatrix4fv(Shader.Uniforms["MVP"], 1, GL_FALSE, Scene.GetDynamicMVP());
	
	//draw functions
	StaticBuffer.Begin();
	StaticBuffer.Draw();
	StaticBuffer.End();
}

void ClearApp() {
	//clear funstions
	StaticBuffer.Close();
	
	memset(keys, 0, 1024);
	Shader.Close();
	LogFile<<"Application: closed"<<endl;
}

int main(int argc, char** argv) {
	LogFile<<"Application: started"<<endl;
	if(!InitApp()) {
		ClearApp();
		glfwTerminate();
		LogFile.close();
		return 0;
	}
	FPSController.Initialize(glfwGetTime());
	while(!glfwWindowShouldClose(window)) {
		FPSController.FrameStep(glfwGetTime());
    	FPSController.FrameCheck();
		RenderStep();
        glfwSwapBuffers(window);
        glfwPollEvents();
	}
	ClearApp();
    glfwTerminate();
    LogFile.close();
}
