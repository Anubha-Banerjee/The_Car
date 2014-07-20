#define FREEGLUT_STATIC

#include<iostream>
#include<GLTools.h>
#include<GL/glut.h>
#include<GLMatrixStack.h>
#include<GLGeometryTransform.h>
#include<stopWatch.h>

#include <GLFrustum.h>
#include "header.h"

using namespace std;

GLBatch light,wheel_in,wheel_out,seats,tail,head,body,ground,trunk,leaves,wallsHut,windowsHut,roofHut,pyramid,bench,county,wings;
GLint shader,locNM,locMVP,locMV,locLight,locTexture,locAmb,locDiff,locSpec,locMaterial,locShine,locSign,locNoShine,locOrg;
GLGeometryTransform transformPipeline;
GLMatrixStack modelViewMatrix,projectionMatrix;
GLuint Tex,Tex_paint,Tex_seat,Tex_tail,Tex_head,Tex_metal,Tex_grass,Tex_bark,Tex_green,Tex_roof,Tex_wall,Tex_brick,Tex_mill,Tex_white;
GLFrame carFrame,worldFrame;

int keyArr[4];
M3DMatrix44f mCameraCar, mCameraWorld;
GLFrustum viewFrustum;	
float len=1280,wide=1024;



// vertex arrays read from obj file (containing only distinct vertices)
component *v,*vn,*vt;

// the final vertics given to openGL
M3DVector3f *Ver;
M3DVector3f *Normals;
M3DVector2f *vTexCoords; 				
class object
{	
public:
		M3DVector3f *vertices;		
		GLfloat lightPosition[3];
		GLfloat ambColor[3];
		GLfloat diffColor[3];
		GLfloat specColor[3];
		GLfloat material[3];
		GLfloat translation1[3];
		GLfloat translation2[3];
		GLfloat translation3[3];
		GLfloat translation4[3];

		GLfloat rotation_x[4];
		GLfloat rotation_y[4];
		GLfloat rotation_z[4];
		GLfloat scale[3];
		GLfloat translateO;
		int noShine;
		float shininess;
		

		object()
		{
			 lightPosition[0]=0;
			 lightPosition[1]=-200;
			 lightPosition[2]=-250;	


			 ambColor[0]=0.01;
			 ambColor[1]=0.01;
			 ambColor[2]=0.01;


			 diffColor[0]=1;
			 diffColor[1]=1;
			 diffColor[2]=1;

			 specColor[0]=0.5;
			 specColor[1]=0.5;
			 specColor[2]=0.5;

			 material[0]=1;
			 material[1]=1;
			 material[2]=1;


	    	 rotation_x[0]=0;
			 rotation_x[1]=1;
			 rotation_x[2]=0;
			 rotation_x[3]=0;


			 rotation_y[0]=0;
			 rotation_y[1]=0;
			 rotation_y[2]=1;
			 rotation_y[3]=0;


			 rotation_z[0]=0;
			 rotation_z[1]=0;
			 rotation_z[2]=0;
			 rotation_z[3]=1;


			 translation1[0]=0;
			 translation1[1]=0;
			 translation1[2]=0;			 

			 translation2[0]=0;
			 translation2[1]=0;
			 translation2[2]=0;


			 
			 translation3[0]=0;
			 translation3[1]=0;
			 translation3[2]=0;			 

			 translation4[0]=0;
			 translation4[1]=0;
			 translation4[2]=0;


			 scale[0]=1;
			 scale[1]=1;
			 scale[2]=1;

			 shininess=120;
			 noShine=0;
			 translateO=0;
		}
};
class car_movement
{
public:
	float travel,prevTravel_x,prevTravel_y,x,y;
	int flag;
	float theta,angleStopped,turn;
	float car_acc,max_car_acc,acc,forw,moving,de_acc;
	int steeringTimeLeft,steeringTimeRight,inc,org_z;

	car_movement()
	{
	
		travel=0;
		prevTravel_x=0;
		prevTravel_y=0;
		flag=0;
		theta=0;
		x=0;
		y=0;
		angleStopped=0;
		car_acc=0;
		max_car_acc=2;
		acc=0.2;
		inc=0;
		org_z=0;
		forw=0;
		moving=0;
		de_acc=0.07;
		turn=-180;
		steeringTimeLeft=0;
		steeringTimeRight=0;
	}
	
	void carMove()
	{
		

			if(keyArr[0]==1)					//A
			{
				{
					steeringTimeLeft=steeringTimeLeft+1;
				
					if(steeringTimeLeft>20)						//it takes a little time you have to to rotate the steering wheel for a certain time before your car starts turning, so thats the steeringTime all about
					{
						turn=turn+2;
						
					}				
				}
			}	
			else
				steeringTimeLeft=0;


			if(keyArr[1]==1)					//D
			{

				steeringTimeRight=steeringTimeRight+1;
			
				if(steeringTimeRight>20)
				{
					turn=turn-2;
					
				}
				
			}
			else
				steeringTimeRight=0;


			if(keyArr[2]==1)					//W
			{
				if(keyArr[1]!=1&&keyArr[0]!=1)					
				{			
					travel=travel+car_acc;

					if(car_acc<max_car_acc)
						car_acc=car_acc+2;

					inc=inc+8;
					angleStopped=turn;					
					
				}	

			}
	}

	void ifStop()
	{
		if(moving==0)											//deaccelerate if car stopped
		{			
				if(car_acc>0)
				{
					travel=travel+car_acc;
					car_acc=car_acc-0.2;
					inc=inc+4;
				}
		}

			
	}
	void computePosition()
	{
		if(flag==1)
		{	
			flag=0;
			travel=0;
			prevTravel_x=x;
			prevTravel_y=y;
		}
		

		theta=3.141*angleStopped/180;
		x=travel*sin(theta)+prevTravel_x;
		y=travel*cos(theta)+prevTravel_y;	

	}


};

car_movement red_car,blue_car ;

bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == NULL) 
		return false;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
				 eFormat, GL_UNSIGNED_BYTE, pBits);
	
    free(pBits);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
       glGenerateMipmap(GL_TEXTURE_2D);    
	return true;
}
void freePointers()
{
	free(v);
	free(vn);
	free(vt);
	free(Ver);
	free(Normals);
	free(vTexCoords);

}
void fillBuffer(char fname[40],GLBatch *batch)
{

	FILE *fp;
	fp=fopen(fname,"r+");
	int total_ver=loadMesh(fp);	
	fclose(fp);
	GLBatch b;


	//batch->Vertex3f(0,0,0);


	batch->Begin(GL_TRIANGLES,total_ver,1);
	batch->CopyVertexData3f(Ver);
	batch->CopyTexCoordData2f(vTexCoords,0);
	batch->CopyNormalDataf(Normals);	
	batch->End();


	freePointers();
}
void drawPoint(float x, float y,float z)
{
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex3f(x, y,z);
	glEnd();
	glFlush(); 
}


void setup()
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	FILE *fp;

	keyArr[0]=0;
	keyArr[1]=0;
	keyArr[2]=0;
	keyArr[3]=0;

	glClearColor(0.4,0.4,0.9,1);
	worldFrame.RotateWorld(-0.1,1,0,0);	
	worldFrame.MoveForward(-3000);
	worldFrame.MoveUp(60);
	

	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
		
	viewFrustum.SetPerspective(35.0f, float(len)/float(wide),1, 5000);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

	int total_ver;
	fillBuffer("exterior.obj",&body);

	fillBuffer("wheel_int.obj",&wheel_in);

	fillBuffer("wheel_out.obj",&wheel_out);
	
	fillBuffer("interior.obj",&seats);

	fillBuffer("taillight.obj",&tail);	

	fillBuffer("headLight.obj",&head);

	fillBuffer("ground.obj",&ground);

	fillBuffer("trunk.obj",&trunk);

	fillBuffer("leaves.obj",&leaves);

	fillBuffer("walls.obj",&wallsHut);

	fillBuffer("roof.obj",&roofHut);

	fillBuffer("kufu.obj",&pyramid);

	fillBuffer("bench.obj",&bench);
	
	fillBuffer("body.obj",&county);

	fillBuffer("wings.obj",&wings);


	
	shader=gltLoadShaderPairWithAttributes("ver.vp","frag.fp",3,GLT_ATTRIBUTE_VERTEX,"vPosition",GLT_ATTRIBUTE_NORMAL,"vNormal",GLT_ATTRIBUTE_TEXTURE0,"vTexture");
	locNM=glGetUniformLocation(shader,"normalMatrix");
	locMVP=glGetUniformLocation(shader,"mvpMatrix");
	locMV=glGetUniformLocation(shader,"mvMatrix");
	locLight=glGetUniformLocation(shader,"lightPosition");
	locTexture=glGetUniformLocation(shader,"colorMap2D");
	locDiff=glGetUniformLocation(shader,"diffLight");
	locAmb=glGetUniformLocation(shader,"ambLight");
	locSpec=glGetUniformLocation(shader,"specLight");
	locShine=glGetUniformLocation(shader,"shininess");
	locMaterial=glGetUniformLocation(shader,"material");
	locMaterial=glGetUniformLocation(shader,"material");
	locSign=glGetUniformLocation(shader,"sign");
	locNoShine=glGetUniformLocation(shader,"noShine");
	locOrg=glGetUniformLocation(shader,"translateToOrg");
	
	
	glGenTextures(1,&Tex_paint);
	glBindTexture(GL_TEXTURE_2D,Tex_paint);	
	LoadTGATexture("red.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);
	
	glGenTextures(1,&Tex_white);
	glBindTexture(GL_TEXTURE_2D,Tex_white);	
	LoadTGATexture("white.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);

	glGenTextures(1,&Tex);
	glBindTexture(GL_TEXTURE_2D,Tex);	
	LoadTGATexture("wheel.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);


	glGenTextures(1,&Tex_seat);
	glBindTexture(GL_TEXTURE_2D,Tex_seat);	
	LoadTGATexture("seat.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);



	glGenTextures(1,&Tex_tail);
	glBindTexture(GL_TEXTURE_2D,Tex_tail);	
	LoadTGATexture("tail.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);

	glGenTextures(1,&Tex_head);
	glBindTexture(GL_TEXTURE_2D,Tex_head);	
	LoadTGATexture("blue.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);


	glGenTextures(1,&Tex_metal);
	glBindTexture(GL_TEXTURE_2D,Tex_metal);	
	LoadTGATexture("metal.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);

	

	glGenTextures(1,&Tex_grass);
	glBindTexture(GL_TEXTURE_2D,Tex_grass);	
	LoadTGATexture("bigrass.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);

	glGenTextures(1,&Tex_bark);
	glBindTexture(GL_TEXTURE_2D,Tex_bark);	
	LoadTGATexture("wood.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);


	
	glGenTextures(1,&Tex_roof);
	glBindTexture(GL_TEXTURE_2D,Tex_roof);	
	LoadTGATexture("roof.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);


	
	glGenTextures(1,&Tex_wall);
	glBindTexture(GL_TEXTURE_2D,Tex_wall);	
	LoadTGATexture("wall.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);
	

	glGenTextures(1,&Tex_brick);
	glBindTexture(GL_TEXTURE_2D,Tex_brick);	
	LoadTGATexture("brick.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);


	
	glGenTextures(1,&Tex_mill);
	glBindTexture(GL_TEXTURE_2D,Tex_mill);	
	LoadTGATexture("windmill.tga", GL_LINEAR, GL_LINEAR, GL_REPEAT);

	glEnable(GL_BLEND);
 
}
void change()
{
	glUniformMatrix3fv(locNM,1,GL_FALSE,transformPipeline.GetNormalMatrix());
	glUniformMatrix4fv(locMV,1,GL_FALSE,transformPipeline.GetModelViewMatrix());
	glUniformMatrix4fv(locMVP,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
}

void bindDraw(GLBatch *batch,GLuint texName)
{
	glBindTexture(GL_TEXTURE_2D,texName);	
	batch->Draw();
}


void setUniforms(object obj)
{
	modelViewMatrix.PushMatrix();

	glUniform3fv(locLight,1,obj.lightPosition);
	glUniform3fv(locAmb,1,obj.ambColor);
	glUniform3fv(locDiff,1,obj.diffColor);
	glUniform3fv(locSpec,1,obj.specColor);
	glUniform1f(locShine,obj.shininess);
	glUniform3fv(locMaterial,1,obj.material);
	glUniform1i(locTexture,0);
	glUniform1i(locSign,-1);	
	glUniform1f(locNoShine,obj.noShine);

	M3DMatrix44f translateOrg={
							1,0,0,0,
							0,1,0,0,
							0,0,1,0,
							0,obj.translateO,0,1,	
							};


	glUniformMatrix4fv(locOrg,1,GL_FALSE,translateOrg);

	modelViewMatrix.Scale(obj.scale[0],obj.scale[1],obj.scale[2]);

	modelViewMatrix.Translate(obj.translation1[0],obj.translation1[1],obj.translation1[2]);	


	modelViewMatrix.Rotate(obj.rotation_y[0],obj.rotation_y[1],obj.rotation_y[2],obj.rotation_y[3]);		
	modelViewMatrix.Translate(obj.translation2[0],obj.translation2[1],obj.translation2[2]);


	modelViewMatrix.Rotate(obj.rotation_z[0],obj.rotation_z[1],obj.rotation_z[2],obj.rotation_z[3]);
	modelViewMatrix.Translate(obj.translation3[0],obj.translation3[1],obj.translation3[2]);

	modelViewMatrix.Rotate(obj.rotation_x[0],obj.rotation_x[1],obj.rotation_x[2],obj.rotation_x[3]);	
	modelViewMatrix.Translate(obj.translation4[0],obj.translation4[1],obj.translation4[2]);

	change();
	modelViewMatrix.PopMatrix();
}


void drawCar(car_movement thisCar)
{
	int initialPosition=1500;

	//car
	object car;	
	car.translateO=0;
		
	/*
	car.material[0]=0.8;
	car.material[1]=0;
	car.material[2]=1;
	*/
	
	car.translation1[0]=thisCar.x;
	car.translation1[1]=0;
	car.translation1[2]=initialPosition+thisCar.y;
	

	car.rotation_y[0]=thisCar.turn;

	setUniforms(car);

	bindDraw(&body,Tex_paint);
	bindDraw(&seats,Tex_seat);
	bindDraw(&tail,Tex_tail);	
	bindDraw(&head,Tex_white);
	light.Draw();


	
	//ending of car 	


	//begining of front wheels	
	object wheel_front;
	wheel_front.translateO=thisCar.org_z-13;
	
	/*
	wheel_front.material[0]=0.2;
	wheel_front.material[1]=0.2;
	wheel_front.material[2]=0.2;
	*/

	wheel_front.rotation_x[0]=thisCar.inc*3;
	wheel_front.rotation_y[0]=thisCar.turn;

	wheel_front.translation2[0]=0;
	wheel_front.translation2[1]=thisCar.org_z+13;
	wheel_front.translation2[2]=thisCar.org_z+57;

	wheel_front.translation1[0]=thisCar.x;
	wheel_front.translation1[1]=0;
	wheel_front.translation1[2]=thisCar.y+initialPosition;	
	
	setUniforms(wheel_front);
	bindDraw(&wheel_in,Tex_metal);
	bindDraw(&wheel_out,Tex);
	//ending of front wheels


	//begining of rear wheels	
	object wheel_rear;
	wheel_rear.translateO=thisCar.org_z-13;
	
	wheel_rear.rotation_x[0]=thisCar.inc*3;
	wheel_rear.rotation_y[0]=thisCar.turn;

	wheel_rear.translation2[0]=0;
	wheel_rear.translation2[1]=thisCar.org_z+13;
	wheel_rear.translation2[2]=thisCar.org_z-57;

	wheel_rear.translation1[0]=thisCar.x;
	wheel_rear.translation1[1]=0;
	wheel_rear.translation1[2]=thisCar.y+initialPosition;	
	
	setUniforms(wheel_rear);
	bindDraw(&wheel_in,Tex_metal);
	bindDraw(&wheel_out,Tex);
	//ending of rear wheels	
	
}






void render()
{

	int initialPosition=1500;
	int static turn_wings=0;


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	modelViewMatrix.PushMatrix();	
	
	carFrame.GetCameraMatrix(mCameraCar);
	worldFrame.GetCameraMatrix(mCameraWorld);

	
	modelViewMatrix.PushMatrix();
	modelViewMatrix.MultMatrix(mCameraWorld);
	
	red_car.carMove();
	red_car.ifStop();

	glUseProgram(shader);

	

	//ground
	
	object groundP;

	groundP.scale[0]=10;
	groundP.scale[1]=1;
	groundP.scale[2]=4;

	/*
	groundP.material[0]=0.1; 
	groundP.material[1]=0.8;
	groundP.material[2]=0.2;	
	*/

	groundP.noShine=1;
	setUniforms(groundP);
	bindDraw(&ground,Tex_grass);

	//end of ground
	

	//the trees
	
	object trees;

	/*
	trees.material[0]=0.5;
	trees.material[1]=1;
	trees.material[2]=0.2;
	*/
	
	trees.noShine=1;
	setUniforms(trees);
	bindDraw(&trunk,Tex_bark);

	
	bindDraw(&leaves,Tex_grass);

	//end of trees
	

	//the bench
	
	object seating;
	seating.scale[0]=20;
	seating.scale[1]=20;
	seating.scale[2]=20;

	seating.rotation_y[0]=-60;

	seating.translation1[0]=20;
	seating.translation1[2]=-20;

	seating.noShine=1;
	setUniforms(seating);
	bindDraw(&bench,Tex_bark);


	//end of bench
	


	//the hut

	object hut;

	hut.scale[0]=1;
	hut.scale[1]=1;
	hut.scale[2]=1;

	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(1.7,1.7,1.7);

	hut.translation1[0]=-100;
	hut.translation1[1]=0;
	hut.translation1[2]=1000;
	

	hut.rotation_y[0]=-45;

	setUniforms(hut);
	bindDraw(&wallsHut,Tex_wall);
	bindDraw(&roofHut,Tex_roof); 
	modelViewMatrix.PopMatrix();
	//end of hut


	//the house

	object house;

	house.scale[0]=2;
	house.scale[1]=2;
	house.scale[2]=2;

	
	house.translation1[0]=-400;
	house.translation1[1]=0;
	house.translation1[2]=0;
	

	setUniforms(house);
	bindDraw(&county,Tex_mill);
	
	//end of house


	
	//the wings
	object pankhe;

	pankhe.scale[0]=2;
	pankhe.scale[1]=2;
	pankhe.scale[2]=2;

	pankhe.rotation_z[0]=turn_wings;
	pankhe.rotation_y[0]=-180;

	turn_wings=turn_wings+2;

	
	pankhe.translation1[0]=-400;
	pankhe.translation1[1]=100;
	pankhe.translation1[2]=50;
	

	
	setUniforms(pankhe);
	bindDraw(&wings,Tex_head);
	
	//end of wings


	//the pyramid

	object kufu;

	kufu.scale[0]=2;
	kufu.scale[1]=2;
	kufu.scale[2]=2;

	
	/*
	hut.material[0]=0.6;
	hut.material[1]=0.3;
	hut.material[2]=0.2;
	*/

	kufu.translation1[0]=300;
	kufu.translation1[1]=0;
	kufu.translation1[2]=300;
	

	kufu.rotation_y[0]=45;

	setUniforms(kufu);
	bindDraw(&pyramid,Tex_brick);
	//end of pyramid

	

	modelViewMatrix.PushMatrix();
	modelViewMatrix.MultMatrix(mCameraCar);
	

	red_car.computePosition();
	drawCar(red_car);


	modelViewMatrix.PopMatrix();					
	modelViewMatrix.PopMatrix();						
	modelViewMatrix.PopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();


	red_car.moving=0;
	
}

int time=0;
float rotn=0.01,forw=0;
void keys(int key,int x,int y)
{

    worldFrame.RotateWorld(+0.1,1,0,0);	
    if(key==GLUT_KEY_UP)
    {
        worldFrame.MoveForward(forw);		
        forw=forw+0.8;

    }


    if(key==GLUT_KEY_DOWN)
    {
        worldFrame.MoveForward(-forw);
        forw=forw+0.8;

    }
    worldFrame.RotateWorld(-0.1,1,0,0);	


    if(key==GLUT_KEY_RIGHT)
    {
        {
            worldFrame.RotateWorld(-rotn,0,1,0);		
        }
    }
    if(key==GLUT_KEY_LEFT)
    {
        worldFrame.RotateWorld(rotn,0,1,0);		
    }

}

unsigned char prev_key='a';


void keyPressed (unsigned char key, int x, int y)
{
		red_car.moving=1;

		if(key=='a')
			keyArr[0]=1;
		if(key=='d')
			keyArr[1]=1;
		if(key=='w')
			keyArr[2]=1;

		if(prev_key=='a'||prev_key=='d')
		{
			red_car.flag=1;
		}
		prev_key=key;
}

void keyReleased(unsigned char key, int x, int y)
{
		if(key=='a')
			keyArr[0]=0;
		if(key=='d')
			keyArr[1]=0;
		if(key=='w')
			keyArr[2]=0;
}
void main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(len,wide);
	glutInitWindowPosition(0,0);
	glutCreateWindow("table");
	glutDisplayFunc(render);
	glutSpecialFunc(keys);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyReleased);
	

	GLenum err=glewInit();
	if(GLEW_OK!=  err)
	{
		fprintf(stderr,"glew errot: %s\n",glewGetErrorString(err));		

	}

	setup();
	glutMainLoop();

}
