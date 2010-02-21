/*****************************************************************************\
**                                                                           **
** File: pyhungarian.c                                                       **
** Author: Dylan Shell                                     Date: Oct 25 2003 **
** Description: Python bindings for libhungarian, so that you can use Beef   **
**              Gerkey's implementation of the hungarian method.             **
**                                                                           **
\*****************************************************************************/

#include "pyhungarian.h"

static int Hungarian_init(HungObj *self, PyObject* args, PyObject *kwds)
{
    char *mode = "min";
    PyObject *data;
    PyObject *walk;
    PyObject *pos;
    int rows,cols;
    int c,i,j;
    int mode_type;
    static char *kwlist[] = {"matrix","mode"};

	DEBUG_MSG1("Hungarian_init()\n");

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|s", kwlist, &data, &mode))
        return -1;

    if ((strcasecmp(mode,"min") == 0)||(strcasecmp(mode,"minimize") == 0))
        mode_type = HUNGARIAN_MIN;
    else
    if ((strcasecmp(mode,"max") == 0)||(strcasecmp(mode,"maximize") == 0))
        mode_type = HUNGARIAN_MAX;
    else
    {
        PyErr_SetString(PyExc_ValueError, "Invalid mode specified");
        return -1;
    }

    if (!PyList_Check(data))
    {
        PyErr_SetString(PyExc_ValueError, "Input Matrix expected as first parameter");
        return -1;
    }

    rows = PyList_Size(data);
    if (rows==0)
    {
        PyErr_SetString(PyExc_ValueError, "Input Matrix has Invalid Row Size");
        return -1;
    }

    /* Do all checking before any memory allocation to keep it clean */
    for (i = 0;i < rows; i++)
    {
        walk = PyList_GetItem(data,i);
        if ((walk == NULL)||(!PyList_Check(walk)))
        {
            PyErr_SetString(PyExc_ValueError, "Input Matrix expected as first parameter");
            return -1;
        }
        if (i==0)
            cols = PyList_Size(walk);
        else if (cols != PyList_Size(walk))
        {
            PyErr_SetString(PyExc_ValueError, "Input Matrix expected as first parameter");
            return -1;
        }
        for (j = 0;j < cols; j++)
        {
            pos =  PyList_GetItem(walk,j);
            if (!PyInt_Check(pos))
            {
                PyErr_SetString(PyExc_ValueError, "Input Matrix of Integers expected as first parameter");
                return -1;
            }
        }
    }

    /* Allocate a matrix for the input */
    self->utility_matrix = malloc(rows*cols*sizeof(int));

    /* Fill this copy */
    for (i = 0;i < rows; i++)
    {
        walk = PyList_GetItem(data,i);
        for (j = 0;j < cols; j++)
        {
            pos =  PyList_GetItem(walk,j);
            c = PyInt_AsLong(pos);
            self->utility_matrix[i*cols+j] = c;
        }
    }

    hungarian_init(&self->hungarian_obj, self->utility_matrix, rows, cols, mode_type);

	return 0;
}

static void Hungarian_dealloc(HungObj *self)
{
    int i;
	DEBUG_MSG1("Hungarian_dealloc()\n");

    hungarian_fini(&self->hungarian_obj);

    /* Free our internal copy of the utility matrix */
    free(self->utility_matrix);
}

static PyObject* Hungarian_solve(HungObj *self)
{
	DEBUG_MSG1("Hungarian_solve()\n");

	hungarian_solve(&self->hungarian_obj);

	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* Hungarian_print_assignment(HungObj *self)
{
	DEBUG_MSG1("Hungarian_print_assignment()\n");

	hungarian_print_assignment(&self->hungarian_obj);

	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* Hungarian_print_rating(HungObj *self)
{
	DEBUG_MSG1("Hungarian_print_rating()\n");

	hungarian_print_rating(&self->hungarian_obj);

	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* Hungarian_check_feasibility(HungObj *self)
{
	int ret_val;
	DEBUG_MSG1("Hungarian_check_feasibility()\n");

	ret_val = hungarian_check_feasibility(&self->hungarian_obj);

    return Py_BuildValue("i",ret_val);
}

static PyObject* Hungarian_benefit(HungObj *self)
{
	int ret_val;
	DEBUG_MSG1("Hungarian_benefit()\n");

	ret_val = hungarian_benefit(&self->hungarian_obj);

    return Py_BuildValue("i",ret_val);
}

static PyObject* Hungarian_get_assignment(HungObj *self)
{
	PyObject *retObj;
	PyObject *v;
    int i;
	DEBUG_MSG1("Hungarian_get_assignment()\n");

    /* We need to cheat, since the API doesn't deal with this, we'll peek
    ** at the hungarian_obj structure */

    retObj = PyList_New(self->hungarian_obj.m);
    if (retObj != NULL)
    {
        for (i = 0; i < self->hungarian_obj.m; i++)
        {
            v = Py_BuildValue("i",self->hungarian_obj.a[i]);
            PyList_SetItem(retObj,i,v);
        }
    } 

    return retObj;

    /* The returned values are the rows for which the columns have been
    ** assigned, i.e. retObj[i] = j means that the entity represented by
    ** column i has been assigned to the entity represented by row j */
}

// Type method table
static PyMethodDef Hungarian_Methods[] = {
    {"solve",               (PyCFunction)Hungarian_solve,               METH_NOARGS},
    {"print_assignment",    (PyCFunction)Hungarian_print_assignment,    METH_NOARGS},
    {"print_rating",        (PyCFunction)Hungarian_print_rating,        METH_NOARGS},
    {"check_feasibility",   (PyCFunction)Hungarian_check_feasibility,   METH_NOARGS},
    {"benefit",             (PyCFunction)Hungarian_benefit,		        METH_NOARGS},
    {"get_assignment",      (PyCFunction)Hungarian_get_assignment,		METH_NOARGS},
    {NULL, NULL}
};

static PyTypeObject HungarianSolverType= {
    PyObject_HEAD_INIT(NULL)
    0,                          //ob_size
    "hungarian.HungarianSolver",//tp_name
    sizeof(HungObj),            //tp_basicsize
    0,                          //tp_itemsize
    (destructor)Hungarian_dealloc,    //tp_dealloc
    0,                          //tp_print
    0,                          //tp_getattr
    0,                          //tp_setattr
    0,                          //tp_compare
    0,                          //tp_repr
    0,                          //tp_as_number
    0,                          //tp_as_sequence
    0,                          //tp_as_mapping
    0,                          //tp_hash 
    0,                          //tp_call
    0,                          //tp_str
    0,                          //tp_getattro
    0,                          //tp_setattro
    0,                          //tp_as_buffer
    Py_TPFLAGS_DEFAULT,         //tp_flags
    "libhungarian Hungarian Assignment Solver Objects",  // tp_doc 
    0,                          // tp_traverse 
    0,                          // tp_clear 
    0,                          // tp_richcompare 
    0,                          // tp_weaklistoffset 
    0,                          // tp_iter 
    0,                          // tp_iternext 
    Hungarian_Methods,          // tp_methods 
    0,                          // tp_members 
    0,                          // tp_getset 
    0,                          // tp_base 
    0,                          // tp_dict 
    0,                          // tp_descr_get 
    0,                          // tp_descr_set 
    0,                          // tp_dictoffset 
    (initproc)Hungarian_init,   // tp_init 
};  
   
// Module method table         
static PyMethodDef Module_Methods[] = {
    {NULL}
};


#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

// Module initialisation
PyMODINIT_FUNC inithungarian(void)
{
    PyObject *module;

    /* Allow the default type new method (platform indp. we do it here) */
    HungarianSolverType.tp_new = PyType_GenericNew;

    /* Set the PlanType.type to PlanType not NULL (platform indp. way) */
    if (PyType_Ready(&HungarianSolverType) < 0)
            return;

    module = Py_InitModule3("hungarian", Module_Methods, 
								"hungarian module, providing the HungarianSolver type");

    if (module == NULL)
            return;

    /* The module has one class variable*/
    Py_INCREF(&HungarianSolverType);
    PyModule_AddObject(module,"HungarianSolver",(PyObject *)&HungarianSolverType);
}


