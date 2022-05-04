#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include <math.h>
typedef struct {
    PyObject_HEAD
    PyObject* edgeList;
    unsigned int verticesList;
} EdgeListObject;
static PyTypeObject EdgeListType;



static void
EdgeList_dealloc(EdgeListObject *self)
{
    Py_TYPE(self)->tp_free((PyObject *) self);
    Py_DECREF(self->edgeList);
}
static PyObject * 
EdgeList_new(PyTypeObject * type,PyObject* arg,PyObject * kwds){
    EdgeListObject * self;
    self = (EdgeListObject *) type->tp_alloc(type, 0);
    return (PyObject*) self;
}

static int
EdgeList_init(EdgeListObject *self, PyObject *args, PyObject *kwds)
{

    const char * g6;
    if (!PyArg_ParseTuple(args, "s", &g6))
        return -1;
    

    int vertices_number = g6[0] - 63;
    self->edgeList = PyList_New(0);
    self->verticesList = 0;
    
    
    
    for(int i = 0 ; i< vertices_number;i++){

        self->verticesList |= (1<<i);
        
    }
    
    int k = 0, i = 1;
    for(int v = 1; v<vertices_number ;v++){
        for(int u = 0; u < v;u++){
            char c;
            if(k == 0){
                c = g6[i]-63;
                i++;
                k = 6;
            }
            k-=1;
            if( (c & (1<<k)) != 0){
                long first = min(u,v);
                long second = max(u,v);
                PyObject * tuple = PyTuple_Pack(2,  PyLong_FromLong(first),  PyLong_FromLong(second));
                PyList_Append(self->edgeList,tuple);
                Py_DECREF(tuple);
            }
             
        }
    }
    return 0;
}

static PyMemberDef EdgeList_members[] = {
    {"list", T_OBJECT, offsetof(EdgeListObject, edgeList), 0,
     "first name"},
    {NULL}  /* Sentinel */
};

// static PyObject *
// EdgeList_name(EdgeListObject *self, PyObject *Py_UNUSED(ignored))
// {
//     if (self->first == NULL) {
//         PyErr_SetString(PyExc_AttributeError, "first");
//         return NULL;
//     }
//     if (self->last == NULL) {
//         PyErr_SetString(PyExc_AttributeError, "last");
//         return NULL;
//     }
//     return PyUnicode_FromFormat("%S %S", self->first, self->last);
// }
static PyObject * EdgeList_vertices(EdgeListObject * self,PyObject * Py_UNUSED(ignored)){
    PyObject* set = PySet_New(NULL);
    for(int i = 0 ; i<16;i++){
       
        if((self->verticesList & (1 << i)) != 0)
        PySet_Add(set,PyLong_FromLong(i));
    }
    
    return set;
}
static PyObject * EdgeList_number_of_vertices(EdgeListObject * self,PyObject * Py_UNUSED(ignored)){
    int count = 0;
    for(int i = 0 ;i< 16;i++){
        if((self->verticesList & (1 << i)) != 0)count ++;
    }
    return PyLong_FromLong(count);
}
static PyObject * EdgeList_vertex_degree(EdgeListObject * self,PyObject * args){
    int count = 0,vert ;
    
    if (!PyArg_ParseTuple(args, "i", &vert))return NULL;
    for(int i = 0; i < PyList_Size(self->edgeList);i++){
        if ( PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,i),0)) == vert )count ++;
        if ( PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,i),1)) == vert )count ++;

    }
    return PyLong_FromLong(count);
}
static PyObject * EdgeList_vertex_neighbors(EdgeListObject * self,PyObject * args){
    int vert;
    if (!PyArg_ParseTuple(args, "i", &vert))return NULL;
    PyObject* result = PySet_New(NULL);
    for(int i = 0; i < PyList_Size(self->edgeList);i++){
        if ( PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,i),0)) == vert ){
            PySet_Add(result,PyTuple_GetItem(PyList_GetItem(self->edgeList,i),1));
        }
        if ( PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,i),1)) == vert ){
            PySet_Add(result,PyTuple_GetItem(PyList_GetItem(self->edgeList,i),0));
        }

    }
    return result;

}
static PyObject * EdgeList_add_vertex(EdgeListObject * self,PyObject * args){
    int vert;
    
    if (!PyArg_ParseTuple(args, "i", &vert))return NULL;
    self->verticesList|= (1<<vert);
    return PyLong_FromLong(1);
}
static PyObject * EdgeList_delete_vertex(EdgeListObject * self,PyObject * args){
    int vert;
    
    if (!PyArg_ParseTuple(args, "i", &vert))return NULL;
    PyObject * tmp = PyList_New(0);
    for(int j = 0 ; j <PyList_Size(self->edgeList);j++){
        
        //printf("y %d\n",j);
        if ( PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,j),0)) == vert ){

        }
        else if(PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,j),1)) == vert){

        }
        else{
            PyList_Append(tmp,PyList_GetItem(self->edgeList,j));
        }
        
    }
    Py_DECREF(self->edgeList);
    
    self->edgeList = tmp;
   
    
    self->verticesList = self->verticesList & (~(1 << vert));
    return PyLong_FromLong(1);
}
static PyObject * EdgeList_number_of_edges(EdgeListObject * self){
    return PyLong_FromLong(PyList_Size(self->edgeList));
}
static PyObject* EdgeList_edges(EdgeListObject* self, PyObject* Py_UNUSED(ignored))
{
    return PySet_New(self->edgeList);
}
static PyObject* EdgeList_is_edge(EdgeListObject* self, PyObject * args)
{
    int v,u;
    
    if (!PyArg_ParseTuple(args, "ii", &v,&u))return NULL;
    int first = min(u,v);
    int second = max(u,v);
    for(int j = 0 ; j <PyList_Size(self->edgeList);j++){
        if ( PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,j),0)) == first
        &&  PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,j),1)) == second ){
            Py_RETURN_TRUE;
        }
    }
    Py_RETURN_FALSE;
 
}
static PyObject* EdgeList_add_edge(EdgeListObject* self, PyObject * args)
{
    int v,u;
    if (!PyArg_ParseTuple(args, "ii", &v,&u))return NULL;
    int first = min(u,v);
    int second = max(u,v);
    PyObject * tuple = PyTuple_Pack(2,PyLong_FromLong(first),PyLong_FromLong(second));
    PyList_Append(self->edgeList,tuple);
    Py_DECREF(tuple);
    PyList_Sort(self->edgeList);
    return PyLong_FromLong(1);
}
static PyObject* EdgeList_delete_edge(EdgeListObject* self,PyObject * args){
       int v,u;
    
    if (!PyArg_ParseTuple(args, "ii", &v,&u))return NULL;
    int first = min(u,v);
    int second = max(u,v);
    PyObject * tmp = PyList_New(0);
    for(int j = 0 ; j <PyList_Size(self->edgeList);j++){
        if ( PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,j),0)) == first
        &&  PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(self->edgeList,j),1)) == second ){
            
        }
        else{
            PyList_Append(tmp,PyList_GetItem(self->edgeList,j));
        }
    }
    
    Py_DECREF(self->edgeList);
    self->edgeList = tmp;
    return PyLong_FromLong(1);
}
static PyTypeObject EdgeListType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "simple_graphs.EdgeList",
    .tp_doc = "EdgeList object",
    .tp_basicsize = sizeof(EdgeListObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_init = (initproc) EdgeList_init,
    .tp_dealloc = (destructor) EdgeList_dealloc,
    .tp_members = EdgeList_members,
    .tp_new = EdgeList_new
};

static PyObject* EdgeList_complement(EdgeListObject* self, PyObject * args)
{
    EdgeListObject * adjObj = EdgeListType.tp_alloc(&EdgeListType, 0);
    adjObj->edgeList = PyList_New(0);
    adjObj->verticesList = 0;
    PyObject * edges = PySet_New(self->edgeList);
    for(int i = 0 ; i< 16;i++){
        unsigned int v = 0;
        int vert = i;
        
        if((self->verticesList & (1 << i)) == 0)continue;
        adjObj->verticesList |= (1 << vert);
        for(int j = i+1; j <16;j++){
            
            int vert2 = j;
            if((self->verticesList & (1 << j)) == 0)continue;
            int first = min(vert,vert2);
            int second = max(vert,vert2);
            PyObject * tuple = PyTuple_Pack(2,  PyLong_FromLong(first),  PyLong_FromLong(second));
            if(PySet_Contains(edges,tuple) == 0){
                PyList_Append(adjObj->edgeList,tuple);
            }
            Py_DECREF(tuple);
        }
    }
    Py_DECREF(edges);
    return adjObj;
}
static PyMethodDef EdgeList_methods[] = {
    {"vertices", (PyCFunction) EdgeList_vertices, METH_NOARGS,
     "xx"
    },
    {"number_of_vertices", (PyCFunction) EdgeList_number_of_vertices, METH_NOARGS,
     "xx"
    },
    {"vertex_degree", (PyCFunction) EdgeList_vertex_degree, METH_VARARGS,
     "xx"
    },
    {"vertex_neighbors", (PyCFunction) EdgeList_vertex_neighbors, METH_VARARGS,
     "xx"
    },
    {"add_vertex", (PyCFunction) EdgeList_add_vertex, METH_VARARGS,
     "xx"
    },
    {"delete_vertex", (PyCFunction) EdgeList_delete_vertex, METH_VARARGS,
     "xx"
    },
    {"number_of_edges", (PyCFunction) EdgeList_number_of_edges, METH_NOARGS,
     "xx"
    },
    {"edges", (PyCFunction) EdgeList_edges, METH_NOARGS,
     "xx"
    },
    {"is_edge", (PyCFunction) EdgeList_is_edge, METH_VARARGS,
     "xx"
    },
    {"add_edge", (PyCFunction) EdgeList_add_edge, METH_VARARGS,
     "xx"
    },
    {"delete_edge", (PyCFunction) EdgeList_delete_edge, METH_VARARGS,
     "xx"
    },
    {"complement", (PyCFunction) EdgeList_complement, METH_NOARGS,
     "xx"
    },
    {NULL}  /* Sentinel */
};


static PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "simple_graphs",
    .m_doc = "simple graphs module",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_simple_graphs(void)
{
    EdgeListType.tp_methods = EdgeList_methods;
    PyObject *m;
    if (PyType_Ready(&EdgeListType) < 0)
        return NULL;

    m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;
    
    Py_INCREF(&EdgeListType);
    if (PyModule_AddObject(m, "EdgesList", (PyObject *) &EdgeListType) < 0) {
        Py_DECREF(&EdgeListType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}