/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// OpenGL ES 2.0 code

#include <jni.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "BulletPhysics/btBulletDynamicsCommon.h"

#define  LOG_TAG    "bulletphysicstest"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// BulletPhysics variables
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btBroadphaseInterface* overlappingPairCache;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

btAlignedObjectArray<btCollisionShape*> collisionShapes;

void addBox(btVector3 position) {
    //create a dynamic rigidbody

    btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
    //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    collisionShapes.push_back(colShape);

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar	mass(1.f);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass,localInertia);

    startTransform.setOrigin(position);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
}

// end


static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

static char* gVertexShader;
static char* gFragmentShader;
/*
static const char gVertexShader[] = 
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";
static const char gFragmentShader[] = 
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "}\n";

*/


char* loadShaderFile(const char* filename, AAssetManager* mgr) {
    //load shaders from assets
    //AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_UNKNOWN);
    if (NULL == asset) {
        LOGE("_ASSET_NOT_FOUND_");
        return NULL;
    }
    LOGI("asset : %x", asset);

    // jni asset manager issue
    // new-delete pair causes error
    // use malloc-free pair to dynamic (de)allocation
    long size = AAsset_getLength(asset);
    char* fileContents = (char*) malloc (sizeof(char)*(size + 1));
    AAsset_read (asset,fileContents,size);
    fileContents[size] = '\0';
    //LOGI("fileContents : %x", fileContents);
    LOGI("filename : %s \n fileContents : %s \n", filename, fileContents);
    //free(fileContents);
    AAsset_close(asset);

    return fileContents;
}

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint gProgram;
GLuint gvPositionHandle;

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);
    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
            gvPositionHandle);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");

    ///-----initialization_start-----

    	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
        collisionConfiguration = new btDefaultCollisionConfiguration();

    	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
        dispatcher = new	btCollisionDispatcher(collisionConfiguration);

    	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
        overlappingPairCache = new btDbvtBroadphase();

    	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
        solver = new btSequentialImpulseConstraintSolver;

        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);

    	dynamicsWorld->setGravity(btVector3(0,-10,0));

        ///create a few basic rigid bodies
    	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));

    	//keep track of the shapes, we release memory at exit.
    	//make sure to re-use collision shapes among rigid bodies whenever possible!
    //	btAlignedObjectArray<btCollisionShape*> collisionShapes;

    	collisionShapes.push_back(groundShape);

    	btTransform groundTransform;
    	groundTransform.setIdentity();
    	groundTransform.setOrigin(btVector3(0,-50,0));

    	{
    		btScalar mass(0.);

    		//rigidbody is dynamic if and only if mass is non zero, otherwise static
    		bool isDynamic = (mass != 0.f);

    		btVector3 localInertia(0,0,0);
    		if (isDynamic)
    			groundShape->calculateLocalInertia(mass,localInertia);

    		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
    		btRigidBody* body = new btRigidBody(rbInfo);



    		//add the body to the dynamics world
    		dynamicsWorld->addRigidBody(body);
    	}


    	{

    		addBox(btVector3(2,10,0));
    		addBox(btVector3(2,15,0));
            addBox(btVector3(2,19,0));
            addBox(btVector3(5,10,0));
            addBox(btVector3(1,10,0));
            addBox(btVector3(1,10,0));
            addBox(btVector3(1,22,0));
            addBox(btVector3(1,25,3));
            addBox(btVector3(1,27,1));
            addBox(btVector3(1,35,2));

    	}




    return true;
}

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f };

void getCubeFromVertices(btCollisionObject* obj, GLfloat* vertexList) {

    /*
     vertex index = 0, pos = 1.000000,1.000000,1.000000
     vertex index = 1, pos = -1.000000,1.000000,1.000000
     vertex index = 2, pos = 1.000000,-1.000000,1.000000
     vertex index = 3, pos = -1.000000,-1.000000,1.000000
     vertex index = 4, pos = 1.000000,1.000000,-1.000000
     vertex index = 5, pos = -1.000000,1.000000,-1.000000
     vertex index = 6, pos = 1.000000,-1.000000,-1.000000
     vertex index = 7, pos = -1.000000,-1.000000,-1.000000

     set triangles : clockwise

     1 : 6 2 4 0 4 2
     2 : 0 1 4 5 4 1
     3 : 3 7 1 5 1 7
     4 : 6 7 2 3 2 7
     5 : 2 3 0 1 0 3
     6 : 7 6 5 4 5 6

     */


    //DISABLE_SIMULATION
    btBoxShape* boxShape = dynamic_cast<btBoxShape *>(obj->getCollisionShape());
    btRigidBody* body = btRigidBody::upcast(obj);
    if (!(body && body->getMotionState())) return;

    float invMass = body->getInvMass();
    //NSLog(@"getInvMass = %f", invMass);

    btVector3 vertex[8];
    int numofvertex = boxShape->getNumVertices();
    for(int i=0;i<numofvertex;i++){
        boxShape->getVertex(i, vertex[i]);
        /*
        btVector3 vertexBasis;
        boxShape->getVertex(i, vertexBasis);
        vertex[i] = trans*vertexBasis;
         */
        //vertex[i];
        //btVector3 vertexAfterTransform = trans*vertexBasis;

        //NSLog(@"vertex index = %d, pos = %f,%f,%f",i,vertexBasis.getX(),vertexBasis.getY(),vertexBasis.getZ());
        //NSLog(@"vertex index = %d, pos = %f,%f,%f",i,vertexAfterTransform.getX(),vertexAfterTransform.getY(),vertexAfterTransform.getZ());
    }

    // TODO : add 3d model objective file parser

    GLint vertexMapList[6][6] = {
        {6, 2, 4, 0, 4, 2},
        {0, 1, 4, 5, 4, 1},
        {3, 7, 1, 5, 1, 7},
        {6, 7, 2, 3, 2, 7},
        {2, 3, 0, 1, 0, 3},
        {7, 6, 5, 4, 5, 6}
    };

    GLfloat normalList[6][3] = {
        {1.0f, .0f, .0f},
        {.0f, 1.0f, .0f},
        {-1.0f, .0f, .0f},
        {.0f, -1.0f, .0f},
        {.0f, .0f, 1.0f},
        {.0f, .0f, -1.0f}
    };

    btTransform trans;
    body->getMotionState()->getWorldTransform(trans);

    for(int i=0;i<6;i++){
        //NSLog(@"plane index : %d", i);
        for(int j=0;j<6;j++){
            //vertexList
            int vertexIndex = vertexMapList[i][j];
            btVector3 curVertex = vertex[vertexIndex];
            btVector3 curNormal(normalList[i][0], normalList[i][1], normalList[i][2]);
            curVertex = trans*curVertex;
//            curVertex = trans*curVertex;
            if(invMass > 0.000001f) {
                curNormal = trans*curNormal;
            } else {
            //    curNormal *= 1000;
            }
            // get normal with vector dot equation

            //curNormal = trans*curNormal;

            //NSLog(@"vertex index : %d", vertexIndex);
            //int vertexListIndex;
            /*
            btVector3 vertexBasis;
            boxShape->getVertex(i, vertexBasis);
            vertex[i] = trans*vertexBasis;
            */

            //vertex
            vertexList[i*36 + j*6 + 0] = curVertex.getX();
            vertexList[i*36 + j*6 + 1] = curVertex.getY();
            vertexList[i*36 + j*6 + 2] = curVertex.getZ();
            //NSLog(@"vertex : %f, %f, %f", curVertex.getX(), curVertex.getY(), curVertex.getZ());
            //LOGI("vertex : %f, %f, %f", curVertex.getX(), curVertex.getY(), curVertex.getZ());

            //normal
            vertexList[i*36 + j*6 + 3] = curNormal.getX();
            vertexList[i*36 + j*6 + 4] = curNormal.getY();
            vertexList[i*36 + j*6 + 5] = curNormal.getZ();
            //LOGI("normal : %f, %f, %f", curNormal.getX(), curNormal.getY(), curNormal.getZ());
            //NSLog(@"normal : %f, %f, %f", curNormal.getX(), curNormal.getY(), curNormal.getZ());

        }
    }
}

void renderFrame() {

	dynamicsWorld->stepSimulation(1.f/60.f,10);

    glClearColor(0.f, 0.f, 0.f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(gProgram);
    checkGlError("glUseProgram");

    GLfloat gCubeVertexList[216];

    //print positions of all objects
    for (int j=dynamicsWorld->getNumCollisionObjects()-1; j>=0 ;j--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
        	btTransform trans;
        	body->getMotionState()->getWorldTransform(trans);
        	getCubeFromVertices(obj, gCubeVertexList);

        	glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, gCubeVertexList);
        	glEnableVertexAttribArray(gvPositionHandle);
        	glDrawArrays(GL_TRIANGLES, 0, 36);




        }

    }

/*

 *        	glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertexList), gCubeVertexList, GL_STATIC_DRAW);

			glEnableVertexAttribArray(GLKVertexAttribPosition);
			glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
			glEnableVertexAttribArray(GLKVertexAttribNormal);
			glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));

			glBindVertexArrayOES(_vertexArray);

			// Render the object with GLKit
			//[self.effect prepareToDraw];

			// 36 = 216 / 6
			glDrawArrays(GL_TRIANGLES, 0, 36);
 * */

}

extern "C" {
	JNIEXPORT void JNICALL Java_net_dykim1983_bulletphysicsndktest_BulletPhysicsLib_initassets(JNIEnv * env, jobject obj, jobject assetManager);
	JNIEXPORT void JNICALL Java_net_dykim1983_bulletphysicsndktest_BulletPhysicsLib_init(JNIEnv * env, jobject obj,  jint width, jint height);
	JNIEXPORT void JNICALL Java_net_dykim1983_bulletphysicsndktest_BulletPhysicsLib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_net_dykim1983_bulletphysicsndktest_BulletPhysicsLib_initassets(JNIEnv * env, jobject obj, jobject assetManager)
{
    //load shaders from assets
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    gVertexShader = loadShaderFile("vertex.shader", mgr);
    gFragmentShader = loadShaderFile("fragment.shader", mgr);

    //boxPng = loadTextureFile("texture/box1.png", mgr);
    //skyPng = loadTextureFile("texture/sky.png", mgr);
    //boxTexture = loadTextureFile("texture/box1.png", mgr);
    //skyTexture = loadTextureFile("texture/sky.png", mgr);

}
JNIEXPORT void JNICALL Java_net_dykim1983_bulletphysicsndktest_BulletPhysicsLib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_net_dykim1983_bulletphysicsndktest_BulletPhysicsLib_step(JNIEnv * env, jobject obj)
{
    renderFrame();
}
