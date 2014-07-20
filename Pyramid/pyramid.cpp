/*
#define FREEGLUT_STATIC
#include<GLTools.h>
#include<GL/glut.h>

GLBatch triangleBatch;
GLint myShader;
float inc=0.02;

GLfloat verts[][3]=
{
	-0.5,0,0,
	0.5,0,0,
	0,0.5,0,

	-0.5,0,0,
	0.5,0,0,
	0,0.25,0.7,

	0.5,0,0,
	0,0.5,0,
	0,0.25,0.7,


	-0.5,0,0,
	0,0.5,0,
	0,0.25,0.7,


};
void init()
{

		glClearColor(0.6,0.7,0.8,1);
		glClear(GL_COLOR_BUFFER_BIT);
		//glutSwapBuffers();


		GLfloat vColor[]=
		{
		
			1,1,1,1,
					0,0.2,0,1,
					1,1,0,1,
		};

	triangleBatch.Begin(GL_TRIANGLES,12,0);
	triangleBatch.CopyVertexData3f(verts);
	triangleBatch.CopyColorData4f(vColor);
	triangleBatch.End();
	myShader=gltLoadShaderPairWithAttributes("vertexShader.vp","fragShader.fp",2,GLT_ATTRIBUTE_VERTEX,"ver",GLT_ATTRIBUTE_COLOR,"color");
}

void render()
{
	
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(myShader);

	triangleBatch.CopyVertexData3f(verts);
	triangleBatch.Draw();
	glutSwapBuffers();
}


void main(int argc,char **argv)
{

	gltSetWorkingDirectory(argv[0]);
	glutInit(&argc,argv);
	
	glutInitWindowPosition(50,50);
	glutInitWindowSize(500,500);
	glutCreateWindow("egypt");
	
	glutDisplayFunc(render);
	
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return ;
	}
	init();
	glutMainLoop();
}

*/