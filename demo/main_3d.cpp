/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, GLEW, standard IO, and strings
#include <SDL.h>
#include <epoxy/gl.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <assert.h>
#include <unordered_map>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
float zNear = 0.1f, zFar = 100.0f;
//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
bool initGL();

//Input handler
void handleKeys( unsigned char key, int x, int y );

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//Shader loading utility programs
void printProgramLog( GLuint program );
void printShaderLog( GLuint shader );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

//Graphics program
// GLuint gProgramID = 0;
// GLint gVertexPos2DLocation = -1;
// GLint vertex_matrix = -1;
// GLuint gVBO = 0;
// GLuint gIBO = 0;

void Perspective(float *a, float fov, float aspect, float zNear, float zFar)
{
    for(int i = 0; i < 16; i++)
        a[i] = 0.0f;

    float f = 1.0f/float(tan(fov / 2.0f * (M_PI / 180.0f)));

    a[0 + 4 * 0] = f / aspect;
    a[1 + 4 * 1] = f;
    a[2 + 4 * 2] = (zNear + zFar) / (zNear - zFar);
    a[2 + 4 * 3] = 2.0f * zNear *+ zFar / (zNear - zFar);
    a[3 + 4 * 2] = -1.0f;
    a[3 + 4 * 3] = 1;
}

void matmul4x4(float* c, float* a, float* b) {
    for (int c_x = 0; c_x < 4; c_x++) {
        for (int c_y = 0; c_y < 4; c_y++) {
            float sum = 0;
            
            for (int i = 0; i < 4; i++) {
                sum += a[i * 4 + c_y] * b[c_x * 4 + i];
            }
            
            c[c_x * 4 + c_y] = sum;
        }
    }
}

void Modelview(float *mv, float scale, float xOff, float yOff, float zOff)
{
    for(int i = 0; i < 16; i++)
        mv[i] = 0.0f;

    mv[0 + 4 * 0] = scale;
    mv[0 + 4 * 3] = xOff;
    mv[1 + 4 * 1] = scale;
    mv[1 + 4 * 3] = yOff;
    mv[2 + 4 * 2] = scale;
    mv[2 + 4 * 3] = zOff;
    mv[3 + 4 * 3] = 1.0f;
}
    
void rot_mat(float* c, float x, float y, float z) {
    float r_x[] = {
      1, 0, 0,0,
      0, cos(x), sin(x),0,
      0, -sin(x), cos(x),0,
      0,0,0,1
    };
    
    float r_y[] = {
      cos(y), 0, -sin(y),0,
      0, 1, 0,0,
      sin(y), 0, cos(y),0,
      0,0,0,1
    };
    
    float r_z[] = {
        cos(z), sin(z), 0,0,
        -sin(z), cos(z), 0,0,
        0, 0, 1,0,
      0,0,0,1
    };
    
    float t[] = {
        0, 0, 0, 0,
        0,0,0,0,
        0,0,0,0
    };
        
    matmul4x4(t, r_x, r_y);
    matmul4x4(c, t, r_z);    
}


class shader_program {
public:
    class single_shader {
    public:
        bool init_fragment(const std::string& src) {
            m_shader = glCreateShader( GL_FRAGMENT_SHADER );
            const GLchar* tmp = static_cast<const GLchar*>(src.c_str());

            glShaderSource(m_shader, 1, &tmp, NULL);
            glCompileShader(m_shader);
            GLint result = GL_FALSE;
            glGetShaderiv( m_shader, GL_COMPILE_STATUS, &result );
            return result == GL_TRUE;
        }

        bool init_vertex(const std::string& src) {
            m_shader = glCreateShader( GL_VERTEX_SHADER );
            const GLchar* tmp = static_cast<const GLchar*>(src.c_str());
            glShaderSource(m_shader, 1, &tmp, NULL);
            glCompileShader(m_shader);
            GLint result = GL_FALSE;
            glGetShaderiv( m_shader, GL_COMPILE_STATUS, &result );
            return result == GL_TRUE;
        }

        GLuint internal_handle() {
            return m_shader;
        }



        GLuint m_shader;
    };

    
        template <typename T>
        typename std::enable_if<std::is_same<int, T>::value,void>::type set_uniform_attr(const std::string& attr, const T x) {
            glUniform1i(m_uniformMap[attr], x);
        }

        template <typename T>
        typename std::enable_if<std::is_same<float, T>::value,void>::type set_uniform_attr(const std::string& attr, const T x) {
            glUniform1f(m_uniformMap[attr], x);
        }

        template <typename T>
        typename std::enable_if<std::is_same<int, T>::value,void>::type set_uniform_attr(const std::string& attr, const T x, const T y) {
            glUniform2i(m_uniformMap[attr], x, y);
        }

        template <typename T>
        typename std::enable_if<std::is_same<float, T>::value,void>::type set_uniform_attr(const std::string& attr, const T x, const T y) {
            glUniform2f(m_uniformMap[attr], x, y);
        }

        template <typename T>
        typename std::enable_if<std::is_same<int, T>::value,void>::type set_uniform_attr(const std::string& attr, const T x, const T y, const T z) {
            glUniform3i(m_uniformMap[attr], x, y, z);
        }

        template <typename T>
        typename std::enable_if<std::is_same<float, T>::value,void>::type set_uniform_attr(const std::string& attr, const T x, const T y, const T z) {
            glUniform3f(m_uniformMap[attr], x, y, z);
        }

        template <typename T>
        typename std::enable_if<std::is_same<int, T>::value,void>::type set_uniform_attr_array(const std::string& attr,const T* x, const int n) {
            glUniform1iv(m_uniformMap[attr], n, x);
        }

        template <typename T>
        typename std::enable_if<std::is_same<float, T>::value,void>::type set_uniform_attr_array(const std::string& attr,const T* x, const int n) {
            glUniform1fv(m_uniformMap[attr], n,  x);
        }

        template <typename T>
        typename std::enable_if<std::is_same<float, T>::value,void>::type set_uniform_attr_4x4_matrix(const std::string& attr, const T* x) {
            glUniformMatrix4fv(m_uniformMap[attr], 1, GL_FALSE, x);
        }
        
        
        
        
    
    std::unordered_map<std::string, GLuint> m_uniformMap;
    
    void add_uniform_attr(const std::string& attr) {
        m_uniformMap.insert_or_assign(attr, glGetUniformLocation(m_id, attr.c_str()));
    }


    
    void attach(single_shader& shader) {
        glAttachShader( m_id, shader.internal_handle() );
    }

    bool link(const std::string& attr, GLuint buffer) {
        auto a = glGetAttribLocation(m_id, attr.c_str());
        if (a != -1) {
            m_links.push_back({a, buffer});
            return true;
        }

        return false;
    }

    std::vector<std::pair<GLint, GLuint>> m_links;

    void init() {
        m_id = glCreateProgram();
    }

    bool finish() {
        glLinkProgram( m_id );
        GLint result = GL_TRUE;
        glGetProgramiv( m_id, GL_LINK_STATUS, &result );
        return result == GL_TRUE;
    }

    void open() {
        glUseProgram(m_id);

        for (auto& l : m_links) {
            glEnableVertexAttribArray(l.first);
            glBindBuffer(GL_ARRAY_BUFFER, l.second);
            glVertexAttribPointer( l.first, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        }
    }

    void close() {
        for (auto& l : m_links) {
            glDisableVertexAttribArray( l.first );
        }

        glUseProgram(NULL);
    }

    void update() {

    }

    GLuint m_id;
};

class mesh {
public:
    struct vertex {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };

    struct triangle {
        GLuint i0;
        GLuint i1;
        GLuint i2;
    };

    void init_buffer() {
        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() *  sizeof(vertex), m_vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangles.size() * sizeof(triangle), m_triangles.data(), GL_STATIC_DRAW);
    }

    void render() {
        m_shaders.open();


        m_shaders.update();

        
        float iden[] = {
            1.0,0.0,0.0,0.0,
            0.0,1.0,0.0,0.0,
            0.0,0.0,1.0,0.0,
            0.0,0.0,0.0,1.0
        };     
        float iden2[] = {
            1.0,0.0,0.0,0.0,
            0.0,1.0,0.0,0.0,
            0.0,0.0,1.0,0.0,
            0.0,0.0,0.0,1.0
        };    
        float iden3[] = {
            1.0,0.0,0.0,0.0,
            0.0,1.0,0.0,0.0,
            0.0,0.0,1.0,0.0,
            0.0,0.0,0.0,1.0
        }; 
        float iden4[] = {
            1.0,0.0,0.0,0.0,
            0.0,1.0,0.0,0.0,
            0.0,0.0,1.0,0.0,
            0.0,0.0,0.0,1.0
        }; 
        float iden5[] = {
            1.0,0.0,0.0,0.0,
            0.0,1.0,0.0,0.0,
            0.0,0.0,1.0,0.0,
            0.0,0.0,0.0,1.0
        }; 
        float iden6[] = {
            1.0,0.0,0.0,0.0,
            0.0,1.0,0.0,0.0,
            0.0,0.0,1.0,0.0,
            0.0,0.0,0.0,1.0
        }; 
        float iden7[] = {
            1.0,0.0,0.0,0.0,
            0.0,1.0,0.0,0.0,
            0.0,0.0,1.0,0.0,
            0.0,0.0,0.0,1.0
        }; 
        
        float pi = 3.1459;
        rot_mat(iden4, 0, m_r, 0);
        
        rot_mat(iden6, 0, m_r, 0);
        
        Modelview(iden2, 1, 2, 0, -2);
        
        m_r += 0.01;
        
        Perspective(iden3, 90.0f, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1, 100);
        
        
        matmul4x4(iden7, iden6, iden2);
        
        matmul4x4(iden5, iden7, iden4);
        matmul4x4(iden, iden3, iden5);
        
        perspective * ((rot * model) * rot)
        
        
        m_shaders.set_uniform_attr_4x4_matrix("m", iden);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glDrawElements(GL_TRIANGLES, m_triangles.size() * 3, GL_UNSIGNED_INT, NULL);

        m_shaders.close();
    }

    float m_r = 0;
    
    GLuint m_vertex_buffer = 0;
    GLuint m_index_buffer = 0;

    shader_program m_shaders;

    std::vector<vertex> m_vertices;
    std::vector<triangle> m_triangles;
};

bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Use OpenGL 3.1 core
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create context
            gContext = SDL_GL_CreateContext( gWindow );
            if( gContext == NULL )
            {
                printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {

                //Use Vsync
                if( SDL_GL_SetSwapInterval( 1 ) < 0 )
                {
                    printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
                }

                //Initialize OpenGL
                if( !initGL() )
                {
                    printf( "Unable to initialize OpenGL!\n" );
                    success = false;
                }
            }
        }
    }

    return success;
}

bool initGL()
{
    return true;
}

void handleKeys( unsigned char key, int x, int y )
{
    //Toggle quad
    if( key == 'q' )
    {
        gRenderQuad = !gRenderQuad;
    }
}

void render(mesh& m)
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );
    glDisable(GL_CULL_FACE);

    //Render quad
    if( gRenderQuad )
    {
        m.render();
    }
}

void close()
{
    //Deallocate program
//     glDeleteProgram( gProgramID );

    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void printProgramLog( GLuint program )
{
    //Make sure name is shader
    if( glIsProgram( program ) )
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a program\n", program );
    }
}

void printShaderLog( GLuint shader )
{
    //Make sure name is shader
    if( glIsShader( shader ) )
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a shader\n", shader );
    }
}

int main( int argc, char* args[] )
{
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Main loop flag
        bool quit = false;

        //Event handler
        SDL_Event e;

        //Enable text input
        SDL_StartTextInput();


        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClearColor( 0.f, 0.f, 0.f, 1.f );


        mesh m;
        m.m_vertices.push_back({-0.5f, -0.25f, 0.0f});
        m.m_vertices.push_back({0.5f, -0.25f, 0.0f});
        m.m_vertices.push_back({-0.5f, 0.25f, -1.0f});
        m.m_vertices.push_back({0.5f, 0.25f, -1.0f});

        m.m_triangles.push_back({0, 1, 3});
        m.m_triangles.push_back({0, 3, 2});

        m.init_buffer();

        shader_program::single_shader vs;
        shader_program::single_shader fs;

        std::string src0 =
            "#version 140\n"
            "in vec3 LVertexPos2D;"
            "uniform mat4 m;"
            "void main() {"
            "vec4 v = vec4(LVertexPos2D.x, LVertexPos2D.y, LVertexPos2D.z, 1);"
            "v = m * v;"
            "gl_Position = v;"
            "}";

        std::string src1 = "#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 0.5, 1.0, 1.0, 1.0 ); }";

        m.m_shaders.init();
        assert(vs.init_vertex(src0));
        assert(fs.init_fragment(src1));


        m.m_shaders.attach(vs);
        m.m_shaders.attach(fs);
        assert(m.m_shaders.finish());
        assert(m.m_shaders.link("LVertexPos2D", m.m_vertex_buffer));

        m.m_shaders.add_uniform_attr("m");


        //While application is running
        while( !quit )
        {
            //Handle events on queue
            while( SDL_PollEvent( &e ) != 0 )
            {
                //User requests quit
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }
                //Handle keypress with current mouse position
                else if( e.type == SDL_TEXTINPUT )
                {
                    int x = 0, y = 0;
                    SDL_GetMouseState( &x, &y );
                    handleKeys( e.text.text[ 0 ], x, y );
                }
            }

            //Render quad
            render(m);

            //Update screen
            SDL_GL_SwapWindow( gWindow );
        }

        //Disable text input
        SDL_StopTextInput();
    }

    //Free resources and close SDL
    close();

    return 0;
}
