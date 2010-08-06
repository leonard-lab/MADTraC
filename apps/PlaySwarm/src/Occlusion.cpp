/* NOTE This code is outdated!  See ../README */

/*
 *  Occlusion.cpp
 *
 *  Created by Daniel Swain on 11/22/09.
 *
 */

#include "Occlusion.h"

#include "MT/MT_Core/gl/glSupport.h"

/* local definitions */
static const double BLIND_RAY_LENGTH = 40.0;

/* TEMPORARY */
static const double a1 = MT_DEG2RAD*  15;
static const double a2 = MT_DEG2RAD*  75;
static const double a3 = MT_DEG2RAD* 135;
static const double a4 = MT_DEG2RAD* 170;
static int g_ix = 0;
static int g_tix = 0;

static bool g_bDrawBlindAngles = false;
static int g_iVisualAngleIndex = 0;

static double* head_x = NULL;
static double* head_y = NULL;
static double* tail_x = NULL;
static double* tail_y = NULL;
static double* head_rel_x = NULL;
static double* head_rel_y = NULL;
static double* tail_rel_x = NULL;
static double* tail_rel_y = NULL;
static double* head_angles = NULL;
static double* tail_angles = NULL;

static unsigned int n_agents = 0;

/* local function declarations */
#pragma mark Local Function Declarations
static void populate_head_tail_buffers(const std::vector<MT_BufferAgent*>& agents);
static void draw_blind_angle(const MT_agent* a);
static void draw_view_angle(unsigned int i, unsigned int j, const MT_Color& color);
static bool in_blind_angle(unsigned int i, unsigned int j, const MT_agent* ai);
static bool cannot_see(unsigned int i, unsigned int j);
static bool in_smallest_arc(double a, double a1, double a2);

/* global functions */
#pragma mark Global Functions

void OCC_nextAOI()
{
    g_tix = g_tix + 1;
    if(g_tix >= n_agents + 1)
    {
        g_tix = 0;
    }
}

void OCC_nextVisualAngle()
{
    g_iVisualAngleIndex++;
    if(g_iVisualAngleIndex >= n_agents + 1)
    {
        g_iVisualAngleIndex = 0;
    }
}

void CalculateAndDrawOcclusions(const std::vector<MT_BufferAgent*>& agents)
{
    n_agents = agents.size();
  
    populate_head_tail_buffers(agents);
  
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glAlphaFunc(GL_GREATER,0.1f);
    glEnable(GL_ALPHA_TEST);
    glDepthFunc(GL_LEQUAL);
  
    glTranslatef(0,0,-0.01);

    MT_agent* ai;
    MT_agent* aj;
    MT_Color line_color = MT_Black;
    for(unsigned int i = 0; i < n_agents; i++)
    {
        ai = agents[i];
        for(unsigned int j = 0; j < n_agents; j++)
        {
            if(i == j)
            {
                continue;
            }

            if(g_iVisualAngleIndex == i ||
               g_iVisualAngleIndex >= n_agents)
            {
                draw_view_angle(i, j, ai->myColor);
            }
      
            if(in_blind_angle(i,j,ai))
            {
                continue;
            }
      
            if(cannot_see(i, j))
            {
                continue;
            }
      
            aj = agents[j];
      
            glLineWidth(2.0);
            MT_DrawLineArrow(ai->x(), ai->y(), aj->x(), aj->y(), MT_Black, 0.2*ai->size, 0.9, true);
            glLineWidth(1.0);
        }

        g_ix = i;
        draw_blind_angle(agents[i]);
    
    }
  
    free(head_x);
    free(head_y);
    free(tail_x);
    free(tail_y);
    free(head_rel_x);
    free(head_rel_y);
    free(tail_rel_x);
    free(tail_rel_y);
    free(head_angles);
    free(tail_angles);
  
    glTranslatef(0,0,0.01);
  
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
  
}

/* local function definitions */
#pragma mark Local Functions

static void populate_head_tail_buffers(const std::vector<MT_BufferAgent*>& agents)
{
  
    head_x = (double *) calloc(n_agents, sizeof(double));
    head_y = (double *) calloc(n_agents, sizeof(double));
    tail_x = (double *) calloc(n_agents, sizeof(double));
    tail_y = (double *) calloc(n_agents, sizeof(double));
    head_rel_x = (double *) calloc(n_agents*n_agents, sizeof(double));
    head_rel_y = (double *) calloc(n_agents*n_agents, sizeof(double));
    tail_rel_x = (double *) calloc(n_agents*n_agents, sizeof(double));
    tail_rel_y = (double *) calloc(n_agents*n_agents, sizeof(double));
    head_angles = (double *) calloc(n_agents*n_agents, sizeof(double));
    tail_angles = (double *) calloc(n_agents*n_agents, sizeof(double));
  
    double x, y, bl, th, costh, sinth;
    double dx, dy, dxr, dyr;
    MT_agent* a; 
    MT_agent* b;
    for(unsigned int i = 0; i < n_agents; i++)
    {
        a = agents[i];
        x = a->x();
        y = a->y();
        bl = a->getSize();
        th = a->gettheta();
        costh = cos(th);
        sinth = sin(th);
        head_x[i] = x + 0.5*bl*costh;
        head_y[i] = y + 0.5*bl*sinth;
        tail_x[i] = x - 0.5*bl*costh;
        tail_y[i] = y - 0.5*bl*sinth;   
    }
  
    for(unsigned int i = 0; i < n_agents; i++)
    {
        a = agents[i];
        th = a->gettheta();
        costh = cos(th);
        sinth = sin(th);
        for(unsigned int j = 0; j < n_agents; j++)
        {
            if(i == j)
            {
                head_angles[i*n_agents + j] = tail_angles[i*n_agents + j] = 0;
                head_rel_x[i*n_agents + j] = head_rel_y[i*n_agents + j] = 0;
                tail_rel_x[i*n_agents + j] = tail_rel_y[i*n_agents + j] = 0;
            }
            b = agents[j];
            dx = head_x[j] - head_x[i];
            dy = head_y[j] - head_y[i];
            /* dr -> (dx + i*dy)*(cos - i*sin) =  dx*cos + dy*sin + i*(dy*cos - dx*sin) */
            dxr = dx*costh + dy*sinth;
            dyr = dy*costh - dx*sinth;
            head_angles[i*n_agents + j] = atan2(dyr,dxr);
            head_rel_x[i*n_agents + j] = dxr;
            head_rel_y[i*n_agents + j] = dyr;
            dx = tail_x[j] - head_x[i];
            dy = tail_y[j] - head_y[i];
            dxr = dx*costh + dy*sinth;
            dyr = dy*costh - dx*sinth;
            tail_angles[i*n_agents + j] = atan2(dyr,dxr);
            tail_rel_x[i*n_agents + j] = dxr;
            tail_rel_y[i*n_agents + j] = dyr;
        }
    }
  
}

static void draw_blind_angle(const MT_agent* a)
{
  
    double bl = a->getSize();
  
    double ba = a->getBlindAngle();
    if(ba == 0)
    {
        return;
    }
  
    double x = a->x();
    double y = a->y();
    double th = a->gettheta();
  
    MT_Color tail_color = MT_LightGray;
  
    glTranslatef(x, y, 0);
    glRotatef(MT_RAD2DEG*th, 0, 0, 1);
    glTranslatef(0.5*bl, 0, 0);

    if(g_bDrawBlindAngles)
    {
        glColor3fv(tail_color);
  
        glBegin(GL_LINE_STRIP);
        glVertex2f(-BLIND_RAY_LENGTH*cos(0.5*ba),
                   BLIND_RAY_LENGTH*sin(0.5*ba));
        glVertex2f(0,0);
        glVertex2f(-BLIND_RAY_LENGTH*cos(0.5*ba),
                   -BLIND_RAY_LENGTH*sin(0.5*ba));
        glEnd();
    }

    /* TEMPORARY */
    if(g_ix == g_tix)
    {
        glColor3fv(MT_Green);
    
        glBegin(GL_LINE_STRIP);
        glVertex2f(BLIND_RAY_LENGTH*cos(a1),BLIND_RAY_LENGTH*sin(a1));
        glVertex2f(0,0);
        glVertex2f(BLIND_RAY_LENGTH*cos(-a1),BLIND_RAY_LENGTH*sin(-a1));
        glEnd();
 
        glBegin(GL_LINE_STRIP);
        glVertex2f(BLIND_RAY_LENGTH*cos(a2),BLIND_RAY_LENGTH*sin(a2));
        glVertex2f(0,0);
        glVertex2f(BLIND_RAY_LENGTH*cos(-a2),BLIND_RAY_LENGTH*sin(-a2));
        glEnd();

        glBegin(GL_LINE_STRIP);
        glVertex2f(BLIND_RAY_LENGTH*cos(a3),BLIND_RAY_LENGTH*sin(a3));
        glVertex2f(0,0);
        glVertex2f(BLIND_RAY_LENGTH*cos(-a3),BLIND_RAY_LENGTH*sin(-a3));
        glEnd();
 
        glBegin(GL_LINE_STRIP);
        glVertex2f(BLIND_RAY_LENGTH*cos(a4),BLIND_RAY_LENGTH*sin(a4));
        glVertex2f(0,0);
        glVertex2f(BLIND_RAY_LENGTH*cos(-a4),BLIND_RAY_LENGTH*sin(-a4));
        glEnd();
    }
 
 
    glTranslatef(-0.5*bl, 0, 0);
    glRotatef(-MT_RAD2DEG*th, 0, 0, 1);
    glTranslatef(-x, -y, 0);
}

static void draw_view_angle(unsigned int i, unsigned int j, const MT_Color& color)
{
  
    MT_Color occ_color = MT_Gray;
    glColor4f(occ_color.R(), occ_color.G(), occ_color.B(), 1.0);
  
    glBegin(GL_LINE_STRIP);
    glVertex2f(head_x[j],head_y[j]);
    glVertex2f(head_x[i],head_y[i]);
    glVertex2f(tail_x[j],tail_y[j]);
    glEnd();
  
    glColor4f(color.R(), color.G(), color.B(), 0.25);
    glBegin(GL_TRIANGLES);
    glVertex2f(head_x[j],head_y[j]);
    glVertex2f(head_x[i],head_y[i]);
    glVertex2f(tail_x[j],tail_y[j]);
    glEnd();
  
}

static bool in_blind_angle(unsigned int i, unsigned int j, const MT_agent* ai)
{
    double beta = MT_PI - 0.5*ai->getBlindAngle();
    return (fabs(head_angles[i*n_agents + j]) > beta) && (fabs(tail_angles[i*n_agents + j]) > beta);
}

static bool cannot_see(unsigned int i, unsigned int j)
{
  
    double ha = head_angles[i*n_agents + j];
    double ta = tail_angles[i*n_agents + j];
  
    int shi, sti, shj, stj;
  
    for(unsigned int m = 0; m < n_agents; m++)
    {
        if((i==m) || (j == m))
        {
            continue;
        }
    
        if(!(in_smallest_arc(ha, head_angles[i*n_agents + m], tail_angles[i*n_agents + m]) 
             && in_smallest_arc(ta, head_angles[i*n_agents + m], tail_angles[i*n_agents + m])))
        {
            continue;
        }
    
        shi = MT_SGN(head_rel_y[m*n_agents + i]);
        sti = MT_SGN(tail_rel_y[m*n_agents + i]);
        shj = MT_SGN(head_rel_y[m*n_agents + j]);
        stj = MT_SGN(tail_rel_y[m*n_agents + j]);
    
        if( abs(shi + sti + shj + stj) == 4 )
        {
            continue;
        }
    
        return true;
    }
  
    return false;
  
}

static bool in_smallest_arc(double a, double a1, double a2)
{
    double A = atan2(sin(a),cos(a));
    double A1 = atan2(sin(a1),cos(a1));
    double A2 = atan2(sin(a2),cos(a2));
    double AA1 = MT_MIN(A1,A2);
    double AA2 = MT_MAX(A1,A2);
  
    if(AA2 - AA1 < MT_PI)
    {
        return ((A < AA2) && (A > AA1));
    }
    else
    {
        return ((A > AA2) && (A < AA1 + 2.0*MT_PI));
    }
}
