#include <Python/Python.h>
#include "db/poi.h"
#include "db/search.h"

using namespace std;
PyObject* wrap_ReadJson(PyObject* self, PyObject* args) {
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;
    ReadJson(filename);

    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* wrap_Search(PyObject* self, PyObject* args) {
    unsigned num;
    double xmin, ymin, xmax, ymax;
    if (!PyArg_ParseTuple(args, "idddd", &num, &xmin, &ymin, &xmax, &ymax)) {
        return NULL;
    }

    const vector<pair<string, int> >& tags = Search(num, xmin, ymin, xmax, ymax);
    PyObject* py_tags = PyList_New(tags.size());
    for (unsigned index = 0; index < tags.size(); ++index) {
        const pair<string, int>& tag = tags.at(index);
        PyObject* py_tag = PyList_New(2);
        PyList_SetItem(py_tag, 0, PyString_FromString(tag.first.data()));
        PyList_SetItem(py_tag, 1, PyInt_FromLong(tag.second));
        PyList_SetItem(py_tags, index, py_tag);
    }
    return py_tags;
}

static PyMethodDef methods[] = {
    {"read_json", wrap_ReadJson, METH_VARARGS, "read POIs from file in json"},
    {"search", wrap_Search, METH_VARARGS, "knn search"},
    {NULL, NULL}
};

PyMODINIT_FUNC initdb() {
    Py_InitModule("db", methods);
}

