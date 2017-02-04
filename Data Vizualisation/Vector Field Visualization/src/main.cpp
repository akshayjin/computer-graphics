#include <GL/glut.h>									//GLUT library
#include "include/UniformGrid.h"
#include "include/StreamlineRenderer.h"
#include "include/GlyphRenderer.h"
#include <iostream>										
#include <fstream>										
#include <cmath>
#include <math.h>

#define GLYPH 0
#define STREAMLINE 1

#define sign(a) ( ( (a) < 0 )  ?  -1   : ( (a) > 0 ) )

int graphType = GLYPH;

float Xmin,Xmax;							//Range of variable x that we sample
float Ymin,Ymax;							//Range of variable y that we sample
int   Nx	= 90, Ny	= 60;							//Number of samples taken along the x and y axes

vector <float> xvalues;
vector <float> yvalues;
vector <float> ux;
vector <float> uy;

UniformGrid			grid(Nx,Ny,Xmin,Ymin,Xmax,Ymax);
StreamlineRenderer	streamlineRenderer;
GlyphRenderer		glyphRenderer;

int					drawing_style = 2;					//Drawing style for streamlines
float				alpha         = 0;					//Rotation of gradient (in radians)
int					seed_density  = 25;					//Density of streamline seeds over the grid
bool				jitter        = false;				//Whether to jitter seeds or use a regular seed-grid

void dataHandling(string fname);

void viewing(int W, int H)								//Window resize function, sets up viewing parameters (GLUT callback function)
{
	glMatrixMode(GL_MODELVIEW);							//1. Set the modelview matrix (including the camera position and view direction)
	glLoadIdentity ();										

	glMatrixMode (GL_PROJECTION);						//2. Set the projection matrix
	glLoadIdentity ();
	gluOrtho2D(Xmin,Xmax,Ymin,Ymax);
	glDisable(GL_DEPTH_TEST);

	glViewport(0,0,W,H);								//3. Set the viewport to the entire size of the rendering window
}




void draw()												//Render the streamlines (GLUT callback function)
{
	if(graphType==GLYPH){
		glyphRenderer.draw(grid);
	}
	else{
		streamlineRenderer.setDrawingStyle((StreamlineRenderer::DRAW_STYLE)drawing_style);
		streamlineRenderer.draw(grid);
	}
}



void createGrid(float alpha)	{						//Create vectors for the grid. For this, we simply sample the 
	for(int i=0;i<grid.numPoints();++i)
	{
	   float p[2];				
	   grid.getPoint(i,p);								//Get coordinates of current grid point
	   float vx = ux[i];
	   float vy = uy[i];
	   if(vx==9999||vx==-9999||vy==9999||vy==-9999){
			 vx=0;vy=0;
		 }
		 else{
			 vx = sign(vx)* abs(log10(abs(vx)));
			 vy = sign(vy)* abs(log10(abs(vy)));
		 }
	   Point3d temp(vx,vy);
	   
		 float scalar = temp.norm();
	   Point3d gradient = temp;
	   Point3d vec;										//Rotate the gradient with an angle alpha, to generate more complex vector fields
	   vec.x = cos(alpha)*gradient.x - sin(alpha)*gradient.y;
	   vec.y = sin(alpha)*gradient.x + cos(alpha)*gradient.y;
	   	   
	   grid.pointVectors().setC0Vector(i,vec);			//Finally, store the rotated gradient as vector values in the grid
	}   
}

void createSeeds()										//Create a set of streamline seeds over the grid
{
   vector<Point3d> seeds;								//The set of seeds to create next
   
   float stepX = (Xmax-Xmin)/seed_density;
   float stepY = (Ymax-Ymin)/seed_density;
   float jitter_factor = 0.3*(stepX+stepY);
   
   for(float x=Xmin;x<Xmax;x+=stepX)
     for(float y=Ymin;y<Ymax;y+=stepY)
	 {
	   Point3d seed(x,y,0);
	   if (jitter)
	   {
	      seed.x += jitter_factor*(float(rand())/RAND_MAX-0.5);	
	      seed.y += jitter_factor*(float(rand())/RAND_MAX-0.5);	
	   }
	   seeds.push_back(seed);							//One more seed
	 }	
	 
   streamlineRenderer.setSeeds(seeds);							//Pass seed-set to streamline renderer
}



void keyboard(unsigned char c,int,int)					//Keyboard callback
{
	switch(c)
	{
	case 'v':											//Change the vector field to visualize
		{
		  alpha = (alpha + M_PI/10);
		  cout << "alpha = " << alpha << endl;
		  createGrid(alpha);
		  break;
		}
	case 'm':
		{
			glyphRenderer.changeScalingMode();
			break;
		}
	case ' ':											//Change the drawing style for streamlines
		{
		  drawing_style = (++drawing_style) % 4;
		  streamlineRenderer.setDrawingStyle((StreamlineRenderer::DRAW_STYLE)drawing_style);
		  break;
		}  	
	case 't':											//Thinner streamlines
		{
		  streamlineRenderer.setThickness(streamlineRenderer.getThickness()/1.1);
		  break;
		}  	
	case 'T':											//Thicker streamlines
		{
		  streamlineRenderer.setThickness(streamlineRenderer.getThickness()*1.1);
		  break;
		}  	
	case 's':											//Less seeds
		{
		  seed_density--; 
		  if (seed_density<1) seed_density=1;
		  createSeeds();
		  break;
		}	
	case 'S':											//More seeds
		{
		  seed_density++; 
		  createSeeds();
		  break;
		}	
	case '-':											//Shorter streamlines
	case '_':
		{
		  streamlineRenderer.setMaxLength(streamlineRenderer.getMaxLength()/1.1);
		  break;
		}  	
	case '+':											//Longer streamlines
	case '=':
		{
		  streamlineRenderer.setMaxLength(streamlineRenderer.getMaxLength()*1.1);
		  break;
		}  	
	case 'j':											//Jitter seeds
		{
		  jitter = !jitter;
		  createSeeds();
		  break;
		}	
	 case 'g':									//Change graph type
	 {
			graphType = (++graphType)%2;
			break;
	 }
	}
				
	glutPostRedisplay();    
}	



int main(int argc,char* argv[])								//Main program
{
   cout<<"Vector visualization (2D uniform grids)"<<endl;
   
   dataHandling(argv[1]);
   
   glutInit(&argc, argv);									//1. Initialize the GLUT toolkit
   glutInitDisplayMode(GLUT_RGB| GLUT_DOUBLE|GLUT_DEPTH);	//2. Ask GLUT to create next windows with a RGB framebuffer and a Z-buffer too
   glutInitWindowSize(500,500);								//3. Tell GLUT how large are the windows we want to create next
   glutCreateWindow("2. Streamlines (2D uniform grid)");		
															//4. Create our window
   srand(time(0));
   
   
   createGrid(alpha);										//5. Create vector values for the grid vertices

   createSeeds();											//6. Create an initial set of seeds over the grid
   
   float step_size = 0.5*((Xmax-Xmin)/Nx+(Ymax-Ymin)/Ny)/2;	//7. Set the integration step size for the streamline to half
   streamlineRenderer.setStepSize(step_size);							//   the size of a grid cell. 
   float max_length = 0.5*((Xmax-Xmin)+(Ymax-Ymin));		//8. Set the maximum length of a streamline to the grid size
   streamlineRenderer.setMaxLength(max_length);						//   (average of the grid width and height)
   streamlineRenderer.setThickness(0.5);
   
   glutKeyboardFunc(keyboard);   
   glutDisplayFunc(draw);									//9. Add a drawing callback to the window	
   glutReshapeFunc(viewing);								//10.Add a resize callback to the window
   glutMainLoop();											//11.Start the event loop that displays the graph and handles window-resize events
   
   return 0;
}

void dataHandling(string fname)
{
	ifstream file ( fname ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
	string x,y,vx,vy;
	int i=0;
	vector <float> xval;
	vector <float> yval;
	vector <float> velX;
	vector <float> velY;
	while ( file.good() )
	{
			 getline ( file, x, ',' ); // read a string until next comma
			 getline ( file, y, ',' );
			 getline ( file, vx, ',' );
			 getline ( file, vy, '\n' );
			 xval.push_back(stof(x));
			 yval.push_back(stof(y));
			 velX.push_back(stof(vx));
			 velY.push_back(stof(vy));
			 if(++i==5400)
				break;
	}
	xvalues = xval;
	yvalues = yval;
	ux = velX;
	uy = velY;
	Xmin = xvalues[0];
	Xmax = xvalues[i-1];
	Ymin = yvalues[0];
	Ymax = yvalues[i-1];
	grid = UniformGrid(Nx,Ny,Xmin,Ymin,Xmax,Ymax);
}
