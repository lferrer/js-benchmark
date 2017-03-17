#include <stdlib.h>
#include <math.h>
#include "esUtil.h"
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#define ES_PI  (3.14159265f)

// Struct used to store the GL-related data
typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Attribute locations
   GLint  positionLoc;
   GLint  normalLoc;
   GLint  texCoordLoc;

   // Shared Uniforms locations
   GLint viewInverseLoc;
   GLint lightWorldPosLoc;
   GLint specularLoc;
   GLint shininessLoc;
   GLint specularFactorLoc;

   // Unique uniforms locations
   GLint colorMultLoc;
   GLint worldLoc;
   GLint worldViewProjectionLoc;
   GLint worldInverseTransposeLoc;

   // Sampler location
   GLint diffuseSampler;

   // Texture handle
   GLuint textureId;

   // Shader buffers
   GLuint vertexObject, normalObject, textureObject, indexObject;   

} UserData;

typedef struct{
    float x, y, z;
    float xRadius, yRadius, zRadius;
    float xClockSpeed, yClockSpeed, zClockSpeed;
    float xClock, yClock, zClock;
    float colorMult[4];
    int modelIndex;

} Instance;

// globals
float g_eyeSpeed          = 0.5f;
float g_eyeHeight         = 2;
float g_eyeRadius         = 9;
int g_maxObjects          = 250000;
int g_numObjects          = 100;
int g_modelsPerBlock      = 50;
int g_targetFrameRate     = 60 - 2;  // add some fudge so browser that runs at 58-59 can still run the test
//var g_avgCounters       = [];
float g_timeCounter       = 0;
int windowWidth           = 1280;
int windowHeight          = 720;
int numSphereVertices     = ( 6 + 1 ) * ( 6 + 1 );
int numSphereIndices      = 6 * 6 * 6;
int numCubeVertices       = 24;
int numCubeIndices        = 36;
GLfloat specular[4]       = {1, 1, 1, 1};
GLfloat shininess         = 50.0f;
GLfloat specularFactor    = 0.2f;
float clock               = 0.0f;
Instance instances[250000];

// Geometry buffers
GLfloat* sphereVertices;
GLfloat* sphereNormals;
GLfloat* sphereTexCoords;
GLushort* sphereIndices;
GLfloat* cubeVertices;
GLfloat* cubeNormals;
GLfloat* cubeTexCoords;
GLushort* cubeIndices;

// pre-allocate a bunch of arrays
ESMatrix projection, world, view, viewProjection;
ESMatrix viewInverse, viewProjectionInverse;
ESVector eyePosition, target, up;
ESMatrix worldInverse, worldInverseTranspose, worldViewProjection;
ESVector lightWorldPos;
ESVector v3t0, v3t1, v3t2;

///
// Load texture from disk
//
GLuint LoadTexture ( char *fileName )
{
   int width,
       height;
   char *buffer = esLoadTGA ( fileName, &width, &height );
   GLuint texId;

   if ( buffer == NULL )
   {
      esLogMessage ( "Error loading (%s) image.\n", fileName );
      return 0;
   }

   glGenTextures ( 1, &texId );
   glBindTexture ( GL_TEXTURE_2D, texId );

   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

   free ( buffer );

   return texId;
}

// Helper functions
float randOne()
{
    return (float)rand() / (float)RAND_MAX;
}

float degToRad(float deg)
{
    return deg * ES_PI / 180.0f;
}

void LoadBuffers ( ESContext *esContext, GLfloat* vertices, 
                   GLfloat* normals, GLfloat* texCoords,
                   GLushort* indices, int numVertices, int numIndices )
{
    UserData *userData = esContext->userData;
    // Load the vertex positions
    glEnableVertexAttribArray ( userData->positionLoc );
    glBindBuffer ( GL_ARRAY_BUFFER, userData->vertexObject );
    glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer ( userData->positionLoc, 3, GL_FLOAT, 
                        GL_FALSE, 0, 0 );
    
    // Load the vertex normals
    glEnableVertexAttribArray ( userData->normalLoc );
    glBindBuffer ( GL_ARRAY_BUFFER, userData->normalObject );
    glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);
    glVertexAttribPointer ( userData->normalLoc, 3, GL_FLOAT, 
                        GL_FALSE, 0, 0 );

    // Load the vertex texture coordinates
    glEnableVertexAttribArray ( userData->texCoordLoc );
    glBindBuffer ( GL_ARRAY_BUFFER, userData->textureObject );
    glBufferData(GL_ARRAY_BUFFER, numVertices * 2 * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer ( userData->texCoordLoc, 2, GL_FLOAT, 
                        GL_FALSE, 0, 0 );

    // Load the vertex indices
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->indexObject );
    glBufferData ( GL_ELEMENT_ARRAY_BUFFER, numIndices * 2 * sizeof(GLushort), indices, GL_STATIC_DRAW );
   
}

int Init ( ESContext *esContext )
{ 
    UserData *userData = esContext->userData;
    GLbyte vShaderStr[] =  
        "uniform mat4 worldViewProjection;                                  \n"
        "uniform vec3 lightWorldPos;                                        \n"
        "uniform mat4 world;                                                \n"
        "uniform mat4 viewInverse;                                          \n"
        "uniform mat4 worldInverseTranspose;                                \n"
        "attribute vec4 position;                                           \n"
        "attribute vec3 normal;                                             \n"
        "attribute vec2 texCoord;                                           \n"
        "varying vec4 v_position;                                           \n"
        "varying vec2 v_texCoord;                                           \n"
        "varying vec3 v_normal;                                             \n"
        "varying vec3 v_surfaceToLight;                                     \n"
        "varying vec3 v_surfaceToView;                                      \n"
        "void main() {                                                      \n"
        "   v_texCoord = texCoord;                                          \n"
        "   v_position = (worldViewProjection * position);                  \n"
        "   v_normal = (worldInverseTranspose * vec4(normal, 0)).xyz;       \n"
        "   v_surfaceToLight = lightWorldPos - (world * position).xyz;      \n"
        "   v_surfaceToView = (viewInverse[3] - (world * position)).xyz;    \n"
        "   gl_Position = v_position;                                       \n"
        "}                                                                  \n";
   
    GLbyte fShaderStr[] = 
        "#ifdef GL_ES                                                           \n"
        "precision mediump float;                                               \n"
        "#endif                                                                 \n"
        "uniform vec4 colorMult;                                                \n"
        "varying vec4 v_position;                                               \n"
        "varying vec2 v_texCoord;                                               \n"
        "varying vec3 v_normal;                                                 \n"
        "varying vec3 v_surfaceToLight;                                         \n"
        "varying vec3 v_surfaceToView;                                          \n"

        "uniform sampler2D diffuseSampler;                                      \n"
        "uniform vec4 specular;                                                 \n"
        "uniform sampler2D bumpSampler;                                         \n"
        "uniform float shininess;                                               \n"
        "uniform float specularFactor;                                          \n"

        "vec4 lit(float l ,float h, float m) {                                  \n"
        "return vec4(1.0,                                                       \n"
        "            max(l, 0.0),                                               \n"
        "            (l > 0.0) ? pow(max(0.0, h), m) : 0.0,                     \n"
        "            1.0);                                                      \n"
        "}                                                                      \n"
        "void main() {                                                          \n"
        "vec4 diffuse = texture2D(diffuseSampler, v_texCoord) * colorMult;      \n"
        "vec3 normal = normalize(v_normal);                                     \n"
        "vec3 surfaceToLight = normalize(v_surfaceToLight);                     \n"
        "vec3 surfaceToView = normalize(v_surfaceToView);                       \n"
        "vec3 halfVector = normalize(surfaceToLight + surfaceToView);           \n"
        "vec4 litR = lit(dot(normal, surfaceToLight),                           \n"
        "                dot(normal, halfVector), shininess);                   \n"
        "gl_FragColor = vec4((                                                  \n"
        "vec4(1,1,1,1) * (diffuse * litR.y                                      \n"
        "                        + specular * litR.z * specularFactor)).rgb,    \n"
        "    diffuse.a);                                                        \n"
        "}                                                                      \n";

    // Load the shaders and get a linked program object
    userData->programObject = esLoadProgram ( vShaderStr, fShaderStr );

    // Get the attribute locations
    userData->positionLoc = glGetAttribLocation ( userData->programObject, "position" );
    userData->normalLoc = glGetAttribLocation ( userData->programObject, "normal" );
    userData->texCoordLoc = glGetAttribLocation ( userData->programObject, "texCoord" );

    // Get the sampler location
    userData->diffuseSampler = glGetUniformLocation ( userData->programObject, "diffuseSampler" );

    // Get the shared uniforms location
    userData->viewInverseLoc = glGetUniformLocation ( userData->programObject, "viewInverse" );
    userData->lightWorldPosLoc = glGetUniformLocation ( userData->programObject, "lightWorldPos" );
    userData->specularLoc = glGetUniformLocation ( userData->programObject, "specular" );
    userData->shininessLoc = glGetUniformLocation ( userData->programObject, "shininess" );
    userData->specularFactorLoc = glGetUniformLocation ( userData->programObject, "specularFactor" );

    // Get the unique uniforms location
    userData->colorMultLoc = glGetUniformLocation ( userData->programObject, "colorMult" );
    userData->worldLoc = glGetUniformLocation ( userData->programObject, "world" );
    userData->worldViewProjectionLoc = glGetUniformLocation ( userData->programObject, "worldViewProjection" );
    userData->worldInverseTransposeLoc = glGetUniformLocation ( userData->programObject, "worldInverseTranspose" );

    // Load the textures
    userData->textureId = LoadTexture ( "basemap.tga" );
   
    if (userData->textureId == 0)
        return FALSE;

    // Create Geometry
    esGenSphere(6, 0.4f, &sphereVertices, &sphereNormals, &sphereTexCoords, &sphereIndices);
    esGenCube(0.8f, &cubeVertices, &cubeNormals, &cubeTexCoords, &cubeIndices); 

    // Setup the instances
    //srand(time(NULL));   // should only be called once
    srand(0); //For testing purposes
    for (int i = 0; i < g_maxObjects; i++){
        instances[i].x = 0;
        instances[i].y = 0;
        instances[i].z = 0;
        instances[i].colorMult[0] = randOne();
        instances[i].colorMult[1] = randOne();
        instances[i].colorMult[2] = randOne();
        instances[i].colorMult[3] = 1;
        instances[i].modelIndex = (int)floor(i / g_modelsPerBlock) % 2;
        instances[i].xRadius = randOne() * 5.0f;
        instances[i].yRadius = randOne() * 5.0f;
        instances[i].zRadius = randOne() * 5.0f;
        instances[i].xClockSpeed = randOne() + 0.5f;
        instances[i].yClockSpeed = randOne() + 0.5f;
        instances[i].zClockSpeed = randOne() + 0.5f; 
        instances[i].xClock = randOne() * ES_PI * 2.0f;
        instances[i].yClock = randOne() * ES_PI * 2.0f;
        instances[i].zClock = randOne() * ES_PI * 2.0f;
    }

    //Generate buffers
    glGenBuffers(1, &userData->vertexObject);
    glBindBuffer ( GL_ARRAY_BUFFER, userData->vertexObject );

    glGenBuffers(1, &userData->normalObject);
    glBindBuffer ( GL_ARRAY_BUFFER, userData->normalObject );

    glGenBuffers(1, &userData->textureObject);
    glBindBuffer ( GL_ARRAY_BUFFER, userData->textureObject );
    
    glGenBuffers(1, &userData->indexObject);
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->indexObject );
   
    return TRUE;     
}

void ShutDown ( ESContext *esContext )
{
    UserData *userData = esContext->userData;

   // Delete texture object
   glDeleteTextures ( 1, &userData->textureId );

   // Delete program object
   glDeleteProgram ( userData->programObject );

}

void Update(ESContext *esContext, float elapsedTime) 
{
    clock += elapsedTime;

    // Make the camera rotate around the scene.
    eyePosition.v[0] = sin(clock * g_eyeSpeed) * g_eyeRadius;
    eyePosition.v[1] = g_eyeHeight;
    eyePosition.v[2] = cos(clock * g_eyeSpeed) * g_eyeRadius;

    // --Update Instance Positions---------------------------------------
    float advance = elapsedTime / 2;
    for (int ii = 0; ii < g_numObjects; ++ii) {
      Instance instance = instances[ii];
      instance.xClock += advance * instance.xClockSpeed;
      instance.yClock += advance * instance.yClockSpeed;
      instance.zClock += advance * instance.zClockSpeed;
      instance.x = sin(instance.xClock) * instance.xRadius;
      instance.y = sin(instance.yClock) * instance.yRadius;
      instance.z = cos(instance.zClock) * instance.zRadius;
    }
  }

void Draw ( ESContext *esContext )
{
    UserData *userData = esContext->userData;

    // Set the viewport
    glViewport ( 0, 0, esContext->width, esContext->height );   
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glClearColor(1,1,1,0);    

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);    

    // Compute a projection and view matrices.
    esMatrixLoadIdentity(&projection);
    esPerspective(&projection, degToRad(60), (float)windowWidth / (float)windowHeight, 1, 5000);
    esLookAt(&view, &eyePosition, &target, &up);
    esMatrixMultiply(&viewProjection, &view, &projection);
    esMatrixInverse(&viewInverse, &view);
    esMatrixInverse(&viewProjectionInverse, &viewProjection);

    // Put the light near the camera
    esGetAxis(&v3t0, &viewInverse, 0); // x
    esGetAxis(&v3t1, &viewInverse, 1); // y
    esGetAxis(&v3t2, &viewInverse, 2); // z
    esVectorScale(&v3t0, &v3t0, 10.0f);
    esVectorScale(&v3t1, &v3t1, 10.0f);
    esVectorScale(&v3t2, &v3t2, 10.0f);    
    esAddVector(&lightWorldPos, &eyePosition, &v3t0);
    esAddVector(&lightWorldPos, &lightWorldPos, &v3t1);
    esAddVector(&lightWorldPos, &lightWorldPos, &v3t2);

    // Use the program object
    glUseProgram ( userData->programObject );

    // Bind the base map
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, userData->textureId );

    // Set the base map sampler to texture unit to 0
    glUniform1i ( userData->diffuseSampler, 0 );

    int lastModel = -1;
    for (int i = 0; i < g_numObjects; ++i) {
      Instance instance = instances[i];
      int model = instance.modelIndex;
      if (model != lastModel) {
        lastModel = model;
        // Copy the appropriate Geometry
        if (model == 0)
        {
            LoadBuffers(esContext, sphereVertices, sphereNormals,
                        sphereTexCoords, sphereIndices,
                        numSphereVertices, numSphereIndices);
        }
        else
        {
            LoadBuffers(esContext, cubeVertices, cubeNormals,
                        cubeTexCoords, cubeIndices,
                        numCubeVertices, numCubeIndices);
        }
        // Copy the shared uniforms
        glUniformMatrix4fv(userData->viewInverseLoc,	1, GL_FALSE, &viewInverse.m[0][0]);
        glUniform3fv(userData->lightWorldPosLoc, 1, &lightWorldPos.v[0]);
 	    glUniform4fv(userData->specularLoc, 1, &specular[0]);
        glUniform1f(userData->shininessLoc, shininess);
        glUniform1f(userData->specularFactorLoc, specularFactor);
      }
      esMatrixLoadIdentity(&world);
      esTranslate(&world, instance.x, instance.y, instance.z);
      esMatrixMultiply(&worldViewProjection, &world, &viewProjection);
      esMatrixInverse(&worldInverse, &world);
      esMatrixTranspose(&worldInverseTranspose, &worldInverse);
      
      // Copy the unique uniforms 
      glUniform4fv(userData->colorMultLoc, 1, &instance.colorMult[0]);
      glUniformMatrix4fv(userData->worldLoc, 1, GL_FALSE, &world.m[0][0]);
      glUniformMatrix4fv(userData->worldViewProjectionLoc, 1, GL_FALSE, &worldViewProjection.m[0][0]);
      glUniformMatrix4fv(userData->worldInverseTransposeLoc, 1, GL_FALSE, &worldInverseTranspose.m[0][0]);

      // Draw the objects
      int numVertices = model == 0 ? numSphereVertices : numCubeVertices;
      glDrawElements ( GL_TRIANGLES, numVertices, GL_UNSIGNED_SHORT, 0 );      
    }   
}

int main ( int argc, char *argv[] )
{
   //Setup constants
   up.v[0] = 0;
   up.v[1] = 1;
   up.v[2] = 0;

   // Initialize OpenGL ES
   ESContext esContext;
   UserData  userData;

   esInitContext ( &esContext );
   esContext.userData = &userData;

   //Get the size on JS or use the default in C
#ifdef __EMSCRIPTEN__
   windowWidth = EM_ASM_INT_V({ return canvas.clientWidth; });
   windowHeight = EM_ASM_INT_V({ return canvas.clientHeight; });
#endif
   esCreateWindow ( &esContext, "WebGL Benchmark", windowWidth, windowHeight, ES_WINDOW_RGB );
   
   if ( !Init ( &esContext ) )
      return 0;

   esRegisterDrawFunc ( &esContext, Draw );
   esRegisterUpdateFunc (&esContext, Update);
   
   esMainLoop ( &esContext );

   ShutDown ( &esContext );
}
