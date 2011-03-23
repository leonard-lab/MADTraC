#include "glSupport.h"

#include <stdio.h>

void MT_InitGLLists()
{
    
	float i;
	double yy;

	glNewList(MT_DISK, GL_COMPILE);
	glBegin(GL_POLYGON);
	for(i = 0;  i <= 360;  i += 20)
		glVertex2f(cos(i*MT_PI/180.0),sin(i*MT_PI/180.0));
	glEnd();
	glEndList();

	/* note: circle gets more resolution since it is usually drawn larger */
	glNewList(MT_CIRCLE, GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	for (i = 0;  i <= 360;  i += 5)
		glVertex2f(cos(i*MT_PI/180.0),sin(i*MT_PI/180.0));
	glEnd();
	glEndList();

	/* higher resolution version of the disk, used by
	MT_DrawCircle when fill = true */
	glNewList(MT_DISK_HIGH_RES, GL_COMPILE);
	glBegin(GL_POLYGON);
	for (i = 0;  i <= 360;  i += 5)
		glVertex2f(cos(i*MT_PI/180.0),sin(i*MT_PI/180.0));
	glEnd();
	glEndList();

	glNewList(MT_ELLIPSE, GL_COMPILE);
	glBegin(GL_POLYGON);
	for (i = 0; i < 360; i += 30)
		glVertex2f(cos(i*MT_PI/180.0),0.5*sin(i*MT_PI/180.0));
	glEnd();
	glEndList();

	glNewList(MT_FISH, GL_COMPILE);
	glBegin(GL_POLYGON);
	for(i = 0; i < 360; i += 10){
		yy = sin(MT_PI*i/180.0);
		yy = 0.5*yy*yy*MT_SGN(yy);
		glVertex2f(0.35 + 0.65*cos(MT_PI*i/180.0),yy);
	} 
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(-0.35,0);
	glVertex2f(-1.0,0.5);
	glVertex2f(-1.0,-0.5);
	glVertex2f(-0.35,0);
	glEnd();  
	glEndList();

	glNewList(MT_ARROW_NECK, GL_COMPILE);
	glBegin(GL_POLYGON);
	glVertex2f(0,0.0125);
	glVertex2f(0.8,0.0125);
	glVertex2f(0.8,-0.0125);
	glVertex2f(0,-0.0125);
	glEnd();
	glEndList();

	glNewList(MT_ARROW_HEAD, GL_COMPILE);
	glBegin(GL_POLYGON);
	glVertex2f(-0.2, -0.05);
	glVertex2f(0, 0.0);
	glVertex2f(-0.2, 0.05);
	glEnd();
	glEndList();

}  

void MT_GLTranslatefv(float* v)
{
    glTranslatef(*v, *(v+1), *(v+2));
}

void MT_DrawArrow(const MT_R3& center, float length, float orientation, const MT_Color& color, float fixed_width)
{
  
    MT_GLTranslatefv(center);
    glColor3fv(color);
    glRotatef(MT_RAD2DEG*orientation, 0, 0, 1);
  
    MT_DrawArrow(length, fixed_width);
  
    glRotatef(-MT_RAD2DEG*orientation, 0, 0, 1);
    MT_GLTranslatefv(center*(-1.0));
  
}

void MT_DrawArrow(float length, float fixed_width)
{  

    const double base_scale = 5.0;
  
    if(fixed_width > 0)
    {
        // based with size = 2 x width
        glScalef(base_scale*fixed_width,base_scale*fixed_width,1.0);
        glCallList(MT_DISK);
        glScalef(1.0/(base_scale*fixed_width),1.0/(base_scale*fixed_width),1.0);
    
        // draw arrow neck
        glScalef(fabs(length-fixed_width)/0.8,80.0*fixed_width,1.0);
        glCallList(MT_ARROW_NECK);
    
        // draw the arrow head
        glTranslatef(1.0,0,0);
        glCallList(MT_ARROW_HEAD);
        glTranslatef(-1.0,0,0);
        glScalef(0.8/fabs(length-fixed_width),0.0125/(fixed_width),1.0);
        return;
    
    }
  
    MT_DrawUnitArrow(length);
  
}

void MT_DrawUnitArrow(float scale)
{  
    double fscale = scale;
    //float lscale = 0.02;
  
    float ahf = 2.0*fscale/0.5;  
    // draw disk at base of arrow with radius = 5% of length
    float base_size = 0.05*scale;
    glScalef(base_size,base_size,1.0);
    glCallList(MT_DISK);
    glScalef(1.0/base_size,1.0/base_size,1.0);
  
    ahf *= 0.5;
    //const float ff = 40.0;  
  
    // draw arrow neck with width = 2.5% of length (neck length = 80% of total length)
    glScalef(fscale,fscale,1.0);
    glCallList(MT_ARROW_NECK);
  
    // draw the arrow head
    glTranslatef(1.0,0,0);
    glCallList(MT_ARROW_HEAD);
    glTranslatef(-1.0,0,0);
    glScalef(1.0/fscale,1.0/fscale,1.0);
  
}

void MT_DrawCircle(const MT_R3& center,
                   const MT_Color& color,
                   double radius,
                   bool fill)
{
  
    double radius_inv = 1.0/radius;
  
    glColor3fv(color);
  
    MT_GLTranslatefv(center);
  
    glScalef(radius, radius, radius);

    if(fill == MT_NO_FILL)
    {
        glCallList(MT_CIRCLE);
    }
    else
    {
        glCallList(MT_DISK_HIGH_RES);
    }
  
    glScalef(radius_inv, radius_inv, radius_inv);
  
    MT_GLTranslatefv(center*(-1.0));
  
}

void MT_DrawCircle(float xcenter,
                   float ycenter,
                   const MT_Color& color,
                   double radius,
                   bool fill)
{
  
    MT_R3 center(xcenter,ycenter,0);
    MT_DrawCircle(center, color, radius, fill);
  
}

void MT_DrawEllipse(const MT_R3& center, 
                 float majoraxislength, 
                 float minoraxislength, 
                 float orientation, 
                 const MT_Color& color)
{
  
    glColor3fv(color);
    MT_GLTranslatefv(center);
    glRotatef(MT_RAD2DEG*orientation, 0, 0, 1);
    glScalef(majoraxislength, minoraxislength, 1.0);
    glCallList(MT_CIRCLE);
    glScalef(1.0/majoraxislength, 1.0/minoraxislength, 1.0);
    glRotatef(-MT_RAD2DEG*orientation, 0, 0, 1);
    MT_GLTranslatefv(center*(-1.0));
  
}

void MT_DrawLineArrow(double xfrom, 
                   double yfrom, 
                   double xto, 
                   double yto, 
                   const MT_Color& color,
                   double head_size, 
                   double head_at, 
                   bool dashed)
{
    if(dashed)
    {
        glLineStipple(1,0x00FF);
        glEnable(GL_LINE_STIPPLE);
    }
  
    glColor3fv(color);
  
    glBegin(GL_LINE_STRIP);
    glVertex2f(xfrom, yfrom);
    glVertex2f(xto, yto);
    glEnd();
  
    double dx = xto - xfrom;
    double dy = yto - yfrom;
    double th = atan2(dy,dx);
    double a = 0.166*MT_PI;
  
    if(dashed)
    {
        glDisable(GL_LINE_STIPPLE);
    }
  
    glTranslatef(xfrom + head_at*dx, yfrom + head_at*dy, 0);
    glRotatef(180.0+MT_RAD2DEG*th, 0, 0, 1);
  
    glBegin(GL_LINE_STRIP);
    glVertex2f(head_size*cos(a), head_size*sin(a));
    glVertex2f(0,0);
    glVertex2f(head_size*cos(a), -head_size*sin(a));
    glEnd();
  
    glRotatef(-MT_RAD2DEG*th-180.0, 0, 0, 1);
    glTranslatef(-xfrom-head_at*dx, -yfrom-head_at*dy, 0);
  
}


void MT_GLDrawGlyph(unsigned int Glyph, const MT_Color& color, const MT_R3& center, double yaw, 
                 double size, double aspect, double p1, double roll, double pitch)
{
  
    if(roll || pitch)
        printf("Warning:  3D orientations not yet implemented\n");
  
    double fscale = (size);

    if(Glyph == MT_ARROW)
    {
        MT_DrawArrow(center, size, yaw, color, aspect);
        return;
    }
  
    MT_GLTranslatefv(center);
    glColor3fv(color);
    glRotatef(MT_RAD2DEG*yaw, 0, 0, 1);
    glScalef(0.5*fscale, aspect*fscale, fscale);
  
    switch(Glyph){
    case MT_DISK:
    case MT_CIRCLE:
    case MT_FISH:
    case MT_SPHERE:
        glCallList(Glyph);
        break;
      
    default:
        glCallList(MT_ELLIPSE);
      
    }
  
    glScalef(2.0/fscale, 1.0/fscale/aspect, 1.0/fscale);
    glRotatef(-MT_RAD2DEG*yaw, 0, 0, 1);
    MT_GLTranslatefv(center*(-1.0));
  
}


void MT_DrawLineCenterLengthAngleColor(const MT_R3& center, float length, float angle, const MT_Color& color)
{
    
    MT_GLTranslatefv(center);
    glColor3fv(color);
    glRotatef(MT_RAD2DEG*angle, 0, 0, 1);
    glScalef(length,length,1.0);
  
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.5,0.0);
    glVertex2f(0.5,0.0);
    glEnd();
  
    glScalef(1.0/length,1.0/length,1.0);
    glRotatef(-MT_RAD2DEG*angle,0,0,1);
    MT_GLTranslatefv(center*(-1.0));
  
}

void MT_DrawLineFromTo(float xfrom, float yfrom, float xto, float yto, const MT_Color& color)
{
  
    glColor3fv(color);
  
    glBegin(GL_LINE_STRIP);
    glVertex2f(xfrom, yfrom);
    glVertex2f(xto, yto);
    glEnd();

}

void MT_DrawRectangle(float x, float y, float width, float height, const MT_Color& color)
{
  
    glColor3fv(color);
  
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
  
}

void MT_CalculateAspectRectangle(double ref_width, 
                              double ref_height, 
                              double* xmin, 
                              double* xmax, 
                              double* ymin, 
                              double* ymax, 
                              double expand_fraction)
{

    double ratio = ref_width/ref_height;
    double pw = *xmax - *xmin;
    double ph = *ymax - *ymin;
    double xcenter, ycenter;
    if(ph == 0){ph = 1;}
    double pr = pw/ph;
  
    /* fix the plot height and center */
    if(pr < ratio){
        pw = ph*ratio;
        xcenter = 0.5*(*xmax + *xmin);
        *xmin = xcenter - 0.5*pw;
        *xmax = xcenter + 0.5*pw;
    }
  
    /* fix the plot width and center */
    if(pr > ratio){
        ph = pw/ratio;
        ycenter = 0.5*(*ymax + *ymin);
        *ymin = ycenter - 0.5*ph;
        *ymax = ycenter + 0.5*ph;
    }
  
    pw = xmax - xmin;
    ph = ymax - ymin;
    *xmin -= expand_fraction*pw;  *xmax += expand_fraction*pw;
    *ymin -= expand_fraction*ph;  *ymax += expand_fraction*ph;
  
}

