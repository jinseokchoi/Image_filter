#include <GL/glut.h> // (or others, depending on the system in use)
#include "sgi.h"
#include <math.h>


GLsizei w,h;
int width, height, depth;
unsigned char *image = NULL;
int Status;

void init (void);
void reshape(int w, int h);
void lineSegment (void);
void mainSelect(int);
void fileSelect(int);
void displaySelect(int);

//Pixel Operation
void NegativeImage();
void GrayScaleImage();
void AdjustingBrightness();
void AdjustingContrast();
void AdjustingSaturation();

//Filtering
void AdjustBlurriness();
void EdgeDetection();
void Sharpening();
void Embossing();

void init (void)  {
	glClearColor (1.0, 1.0, 1.0, 0.0);       // Set display-window color to white.
	glMatrixMode (GL_PROJECTION); // Set projection parameters.
	gluOrtho2D (0.0, 200.0, 0.0, 150.0);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	height = (GLint) h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display(void)		//화면에 항상찍고있어라
{

	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);

	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
	glFlush();
}

void mainSelect(int value)
{
	Status=value;
	switch (value){
	case 666:
		printf("exit\n");
		exit(0);
		break;
	}
}

void fileSelect(int value)		//그림파일 추가
{
	char *filename;
	Status=value;
	switch (value) {
	case 0:
		filename="sample.rgb";
		image = read_sgi(filename, &width, &height, &depth);
		glutReshapeWindow(width,height);
		display();
		break;

	case 1:
		filename="train.bmp";
		image = read_bmp(filename, &width, &height, &depth);
		glutReshapeWindow(width,height);
		display();
		break;
	}
}

void DisplaySelect(int value) //pixel operation select
{
	Status=value;
	switch (value) {
	case 10:
		NegativeImage();
		display();
		break;
	case 11:
		GrayScaleImage();
		display();
		break;
	case 12:
		AdjustingBrightness();
		display();
		break;
	case 13:
		AdjustingContrast();
		display();
		break;
	case 14:
		AdjustingSaturation();
		display();
		break;
	}
}

void FilteringSelect(int value){ //Filtering select
	Status = value;
	switch(value){
	case 20:
		AdjustBlurriness();
		display();
		break;
	case 21:
		EdgeDetection();
		display();
		break;
	case 22:
		Sharpening();
		display();
		break;	
	case 23:
		Embossing();
		display();
		break;
	}
}

int MIN(int a,int b,int c) 
{
	int min=a<b?a:b;
	return min<c?min:c;
}
int MAX(int a,int b,int c)
{
	int max=a>b?a:b;
	return max>c?max:c;
}
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v ) //Color Convert  RGB  HSV
{
	float min, max, delta;
	min = MIN( r, g, b );
	max = MAX( r, g, b );
	*v = max;				//v
	delta = max - min;

	if( max != 0 )
		*s = delta / max;		//s
	
	else {
		//r = g = b = 0		//s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}

	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan
	*h *= 60;				// degrees
	
	if( *h < 0 )
		*h += 360;
}
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v ) //Color Convert  HSV => RGB
{
	int i;
	float f, p, q, t;
	if(s == 0) {
		//achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			//sector 0 to 5
	i = floor( h );
	f = h - i;			//factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch(i) {
	case 0:
		*r = v;
		*g = t;
		*b = p;
		break;
	case 1:
		*r = q;
		*g = v;
		*b = p;
		break;
	case 2:
		*r = p;
		*g = v;
		*b = t;
		break;
	case 3:
		*r = p;
		*g = q;
		*b = v;
		break;
	case 4:
		*r = t;
		*g = p;
		*b = v;
		break;
	default:		// case 5:
		*r = v;
		*g = p;
		*b = q;
		break;
	}
}
//MIN, MAX, RGBtoHSV, HSVtoRGB => AdjustingSaturation

//pixel operation
void NegativeImage() { //inverse pixel components
	int i,j;
	for (j=0;j<height; j++)  {
		for (i=0;i<width*3; i=i+3) {
			image[j*width*3+i+0]=255-image[j*width*3+i+0]; //inverse pixel
			image[j*width*3+i+1]=255-image[j*width*3+i+1];
			image[j*width*3+i+2]=255-image[j*width*3+i+2];
		}
	}
}

void GrayScaleImage() {
	int i, j, r, g, b;
	for (j=0;j<height; j++)  {
		for (i=0;i<width*3; i=i+3) {
			r=image[j*width*3+i+0];
			g=image[j*width*3+i+1];
			b=image[j*width*3+i+2];

			image[j*width*3+i+0]=0.2125*r+0.7154*g+0.0721*b;
			image[j*width*3+i+1]=0.2125*r+0.7154*g+0.0721*b;
			image[j*width*3+i+2]=0.2125*r+0.7154*g+0.0721*b; 
			//Compute luninance L = 0.2125R+0.7154G+0.0721B
		}
	}
}

void AdjustingBrightness() {	//(R, G, B) = (R, G , B) * scale
	int i, j, r, g, b;
	float scale = 1.2;			//1.2는 밝게 0.8 로 설정할경우 어둡게

	for (j=0;j<height; j++)  {
		for (i=0;i<width*3; i=i+3) {
			r=image[j*width*3+i+0]*scale;
			g=image[j*width*3+i+1]*scale;
			b=image[j*width*3+i+2]*scale;

			if(r>255) r=255;		
			if(g>255) g=255;		
			if(b>255) b=255;
			//Must clamp to range (e.g., 0 to 255)

			image[j*width*3+i+0]=r;
			image[j*width*3+i+1]=g;
			image[j*width*3+i+2]=b;
			//이미지에 위에서 연산한 rgb값을 붙여넣음
		}
	}
}
void AdjustingContrast(){ //R = L + (R-L)*scale 
						  //G = L + (G-L)*scale 
						  //B = L + (B-L)*scale 
						  //Must clamp to range (e.g., 0 to 255)

	int i, j, r, g, b;
	float scale = 1.2; //1.2는 밝게 0.8 로 설정할경우 어둡게
	int L;

	for (j=0;j<height; j++)  {
		for (i=0;i<width*3; i=i+3) {
			L = (0.3*image[j*width*3+i+0]) + (0.59*image[j*width*3+i+1]) +(0.11*image[j*width*3+i+2]);

			r = L + (image[j*width*3+i+0]-L)*scale;
			g = L + (image[j*width*3+i+1]-L)*scale;
			b = L + (image[j*width*3+i+2]-L)*scale;
			//Luminance = 0.30* r + 0.59*g + 0.11*b

			if(r>255) r=255;		
			if(g>255) g=255;		
			if(b>255) b=255;

			image[j*width*3+i+0]=r;
			image[j*width*3+i+1]=g;
			image[j*width*3+i+2]=b;
			//이미지에 위에서 연산한 rgb값을 붙여넣음
		}
	}
}
void AdjustingSaturation(){ //채도
   int i,j;
   float r,g,b;
   float h,s,v;
   float scale = 1.2; //1.2는 밝게 0.8 로 설정할경우 어둡게

   for (j=0;j<height; j++){
      for (i=0;i<width*3; i=i+3){
         r = image[j*width*3+i+0];
         g = image[j*width*3+i+1];
         b = image[j*width*3+i+2];
		 RGBtoHSV(r,g,b,&h,&s,&v); //Color Convert  RGB => HSV
         s *= scale; //값에 따라 밝기 변환
         HSVtoRGB(&r,&g,&b,h,s,v); //Color Convert  HSV => RGB

        
         if (r>255) r=255;   
		 if (r<0) r=0;
         if (g>255) g=255;  
		 if (g<0) g=0;
         if (b>255) b=255;
		 if (b<0) b=0;
		 //Must clamp to range (e.g., 0 to 255)

         image[j*width*3+i+0]=r;
         image[j*width*3+i+1]=g;
         image[j*width*3+i+2]=b;
		 //이미지에 위에서 연산한 rgb값을 붙여넣음
      }
   }
}

// Filtering
void AdjustBlurriness() {
	double filter[3][3]={{0.0625,0.125,0.0625},{0.125,0.25,0.125},{0.0625,0.125,0.0625}}; //Filter sum=1
	unsigned char *timage = (unsigned char *)malloc(width*height*3); //이미지변수 임시 동적할당
	double r, g, b, sumr, sumg, sumb;
	int i, j, k, l, pos;
	double filter_sum=1; 

	for (j=1;j<height-1; j++)  { 
		for (i=1;i<(width-1); i++) { 
			sumr=0; sumb=0; sumg=0; 
			for(k=0;k<3;k++){ 
				for(l=0;l<3;l++){ 
					pos= (j+l-1)*width*3+(i+k-1)*3; 

					r=image[pos+0]; 
					g=image[pos+1]; 
					b=image[pos+2]; 

					sumr += r*filter[k][l]; 
					sumg += g*filter[k][l]; 
					sumb += b*filter[k][l]; 
				} 
			} 
			sumr/=filter_sum; 
			sumg/=filter_sum; 
			sumb/=filter_sum; 

			if ( sumr > 255) sumr=255; 
			if ( sumr < 0.0) sumr=0.0; 
			if ( sumg > 255) sumg=255;
			if ( sumg < 0.0) sumg=0.0; 
			if ( sumb > 255) sumb=255; 
			if ( sumb < 0.0) sumb=0.0; 
			//Must clamp to range (e.g., 0 to 255)

			timage[j*width*3+i*3+0]=sumr; 
			timage[j*width*3+i*3+1]=sumg; 
			timage[j*width*3+i*3+2]=sumb; 
		}
	} 
	for (j=1;j<height-1; j++)  { 
		for (i=1;i<(width-1); i++) { 
			image[j*width*3+i*3+0]=timage[j*width*3+i*3+0]; 
			image[j*width*3+i*3+1]=timage[j*width*3+i*3+1]; 
			image[j*width*3+i*3+2]=timage[j*width*3+i*3+2]; 
			//이미지에 위에서 연산한 rgb값을 붙여넣음
		} 
	} 
}

void EdgeDetection() {
	int filter[3][3]={{-1,-1,-1},{-1,8,-1},{-1,-1,-1}}; 
	unsigned char *timage = (unsigned char *)malloc(width*height*3); //이미지변수 임시 동적할당
	int r, g, b, sumr, sumg, sumb;
	int i, j, k, l, pos;
	int filter_sum=1; 
	for (j=1;j<height-1; j++)  { 
		for (i=1;i<(width-1); i++) { 
			sumr=0; sumb=0; sumg=0; 

			for(k=0;k<3;k++){ 
				for(l=0;l<3;l++){ 
					pos= (j+l-1)*width*3+(i+k-1)*3; 

					r=image[pos+0]; 
					g=image[pos+1]; 
					b=image[pos+2]; 

					sumr += r*filter[k][l]; 
					sumg += g*filter[k][l]; 
					sumb += b*filter[k][l]; 
				} 
			} 
			sumr/=filter_sum; 
			sumg/=filter_sum; 
			sumb/=filter_sum; 

			if ( sumr > 255) sumr=255; 
			if ( sumr < 0.0) sumr=0.0; 
			if ( sumg > 255) sumg=255; 
			if ( sumg < 0.0) sumg=0.0; 
			if ( sumb > 255) sumb=255;
			if ( sumb < 0.0) sumb=0.0; 
			//Must clamp to range (e.g., 0 to 255)

			timage[j*width*3+i*3+0]=sumr; 
			timage[j*width*3+i*3+1]=sumg; 
			timage[j*width*3+i*3+2]=sumb; 
		}
	} 

	for (j=1;j<height-1; j++)  { 
		for (i=1;i<(width-1); i++) { 
			image[j*width*3+i*3+0]=timage[j*width*3+i*3+0]; 
			image[j*width*3+i*3+1]=timage[j*width*3+i*3+1]; 
			image[j*width*3+i*3+2]=timage[j*width*3+i*3+2]; 
			//이미지에 위에서 연산한 rgb값을 붙여넣음
		} 
	} 
}

void Sharpening() {
	int filter[3][3]={{0,-1,0},{-1,5,-1},{0,-1,0}}; 
	unsigned char *timage = (unsigned char *)malloc(width*height*3); //이미지변수 임시 동적할당
	int r, g, b, sumr, sumg, sumb;
	int i, j, k, l, pos; 
	int filter_sum=1; 
	for (j=1;j<height-1; j++)  { 
		for (i=1;i<(width-1); i++) { 
			sumr=0; sumb=0; sumg=0; 
			for(k=0;k<3;k++){ 
				for(l=0;l<3;l++){ 
					pos= (j+l-1)*width*3+(i+k-1)*3; 

					r=image[pos+0]; 
					g=image[pos+1]; 
					b=image[pos+2]; 

					sumr += r*filter[k][l]; 
					sumg += g*filter[k][l]; 
					sumb += b*filter[k][l]; 
				} 
			} 
			sumr/=filter_sum; 
			sumg/=filter_sum; 
			sumb/=filter_sum; 

			if ( sumr > 255) sumr=255;
			if ( sumr < 0.0) sumr=0.0; 
			if ( sumg > 255) sumg=255; 
			if ( sumg < 0.0) sumg=0.0; 
			if ( sumb > 255) sumb=255; 
			if ( sumb < 0.0) sumb=0.0; 
			//Must clamp to range (e.g., 0 to 255)

			timage[j*width*3+i*3+0]=sumr; 
			timage[j*width*3+i*3+1]=sumg; 
			timage[j*width*3+i*3+2]=sumb; 
		}
	} 
	for (j=1;j<height-1; j++)  { 
		for (i=1;i<(width-1); i++) { 
			image[j*width*3+i*3+0]=timage[j*width*3+i*3+0]; 
			image[j*width*3+i*3+1]=timage[j*width*3+i*3+1]; 
			image[j*width*3+i*3+2]=timage[j*width*3+i*3+2]; 
			//이미지에 위에서 연산한 rgb값을 붙여넣음
		} 
	} 
}
void Embossing(){
	int filter[3][3]={{2,0,0},{0,-1,0},{0,0,-1}}; 
	unsigned char *timage = (unsigned char *)malloc(width*height*3); //이미지변수 임시 동적할당
	int r, g, b, sumr, sumg, sumb;
	int i, j, k, l, pos;
	int filter_sum=1;
	GrayScaleImage();
	for (j=1;j<height-1; j++)  { 
		for (i=1;i<(width-1); i++) { 
			sumr=0; sumb=0; sumg=0; 
			for(k=0;k<3;k++){ 
				for(l=0;l<3;l++){ 
					pos= (j+l-1)*width*3+(i+k-1)*3; 

					r=image[pos+0]; 
					g=image[pos+1]; 
					b=image[pos+2]; 

					sumr += r*filter[k][l]; 
					sumg += g*filter[k][l]; 
					sumb += b*filter[k][l]; 
				} 
			} 
			sumr/=filter_sum; 
			sumg/=filter_sum; 
			sumb/=filter_sum; 

			sumr += 127;
			sumg += 127;
			sumb += 127;

			if ( sumr > 255) sumr=255; 
			if ( sumr < 0.0) sumr=0.0; 
			if ( sumg > 255) sumg=255; 
			if ( sumg < 0.0) sumg=0.0; 
			if ( sumb > 255) sumb=255; 
			if ( sumb < 0.0) sumb=0.0; 
			//Must clamp to range (e.g., 0 to 255)

			timage[j*width*3+i*3+0]=sumr; 
			timage[j*width*3+i*3+1]=sumg; 
			timage[j*width*3+i*3+2]=sumb; 
		}
	} 
	for (j=1;j<height-1; j++)  { 
		for (i=1;i<(width-1); i++) { 
			image[j*width*3+i*3+0]=timage[j*width*3+i*3+0]; 
			image[j*width*3+i*3+1]=timage[j*width*3+i*3+1]; 
			image[j*width*3+i*3+2]=timage[j*width*3+i*3+2]; 
			//이미지에 위에서 연산한 rgb값을 붙여넣음
		} 
	} 
}

void main (int argc, char** argv)   {

	int fileMenu, DisplayMenu, mainMenu, FilteringMenu;
	glutInit (&argc, argv); 				// Initialize GLUT.
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); 	// Set display mode.
	//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);


	glutInitWindowPosition (50, 100); 	// Set top-left display-window position.
	glutInitWindowSize (300, 300); 	// Set display-window width and height.
	glutCreateWindow ("An Example OpenGL Program"); // Create display window.

	init ( ); 					// Execute initialization procedure.
	glutDisplayFunc (display); 	// Send graphics to display window.
	glutReshapeFunc(reshape);

	fileMenu = glutCreateMenu(fileSelect);
	glutAddMenuEntry("sample.rgb",0);
	glutAddMenuEntry("train.bmp",1);

	DisplayMenu = glutCreateMenu(DisplaySelect);
	glutAddMenuEntry("Negative Image",10);
	glutAddMenuEntry("GrayScale Image",11);
	glutAddMenuEntry("AdjustingBrightness",12);
	glutAddMenuEntry("AdjustingContrast",13);
	glutAddMenuEntry("AdjustingSaturation",14);

	FilteringMenu =  glutCreateMenu(FilteringSelect);
	glutAddMenuEntry("AdjustBlurriness",20);
	glutAddMenuEntry("EdgeDetection",21);
	glutAddMenuEntry("Sharpening",22);
	glutAddMenuEntry("Embossing",23);


	mainMenu = glutCreateMenu(mainSelect);	
	glutAddSubMenu("File", fileMenu);			
	glutAddSubMenu("Pixel operation", DisplayMenu);
	glutAddSubMenu("Filtering", FilteringMenu);

	glutAddMenuEntry("Quit",666);		
	glutSetMenu(mainMenu);				//오른쪽버튼 누르면 생성
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//오른쪽버튼 누르면 팝업버튼 떠라

	glutMainLoop ( ); 		// Display everything and wait.
}

