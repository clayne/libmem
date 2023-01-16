/*
 *  ----------------------------------
 * |         libmem - by rdbo         |
 * |      Memory Hacking Library      |
 *  ----------------------------------
 */

/*
 * Copyright (C) 2022    Rdbo
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <libmem/libmem.h>
#include <Python.h>
#include <structmember.h>
#include "types.h"

#define DECL_GLOBAL(var) { \
	/* make sure that 'pymod' and 'global' are declared */ \
	global = (PyObject *)PyLong_FromLong((long)var); \
	PyObject_SetAttrString(pymod, #var, global); \
	Py_DECREF(global); \
}

static lm_bool_t
_py_LM_EnumProcessesCallback(lm_process_t *pproc,
			     lm_void_t    *arg)
{
	PyObject *pylist = (PyObject *)arg;
	py_lm_process_obj *pyproc;

	pyproc = (py_lm_process_obj *)PyObject_CallObject((PyObject *)&py_lm_process_t, NULL);
	pyproc->proc = *pproc;

	PyList_Append(pylist, (PyObject *)pyproc);

	return LM_TRUE;
}

static PyObject *
py_LM_EnumProcesses(PyObject *self,
		    PyObject *args)
{
	PyObject *pylist = PyList_New(0);
	if (!pylist)
		return NULL;

	if (!LM_EnumProcesses(_py_LM_EnumProcessesCallback, (lm_void_t *)pylist)) {
		Py_DECREF(pylist); /* destroy list */
		pylist = Py_BuildValue("");
	}

	return pylist;
}

/****************************************/

static PyObject *
py_LM_GetProcess(PyObject *self,
		 PyObject *args)
{
	lm_process_t proc;
	py_lm_process_obj *pyproc;

	if (!LM_GetProcess(&proc))
		return Py_BuildValue("");

	pyproc = (py_lm_process_obj *)PyObject_CallObject((PyObject *)&py_lm_process_t, NULL);
	pyproc->proc = proc;

	return (PyObject *)pyproc;
}

/****************************************/

static PyObject *
py_LM_GetProcessEx(PyObject *self,
		   PyObject *args)
{
	lm_pid_t pid;
	lm_process_t proc;
	py_lm_process_obj *pyproc;

	if (!PyArg_ParseTuple(args, "i", &pid))
		return NULL;

	if (!LM_GetProcessEx(pid, &proc))
		return Py_BuildValue("");

	pyproc = (py_lm_process_obj *)PyObject_CallObject((PyObject *)&py_lm_process_t, NULL);
	pyproc->proc = proc;

	return (PyObject *)pyproc;
}

/****************************************/

static PyObject *
py_LM_FindProcess(PyObject *self,
		  PyObject *args)
{
	lm_char_t         *procstr;
	lm_process_t       proc;
	py_lm_process_obj *pyproc;

#	if LM_CHARSET == LM_CHARSET_UC
	if (!PyArg_ParseTuple(args, "u", &procstr))
			return NULL;
#	else
	if (!PyArg_ParseTuple(args, "s", &procstr))
		return NULL;
#	endif

	if (!LM_FindProcess(procstr, &proc))
		return Py_BuildValue("");

	pyproc = (py_lm_process_obj *)PyObject_CallObject((PyObject *)&py_lm_process_t, NULL);
	pyproc->proc = proc;

	return (PyObject *)pyproc;
}

/****************************************/

static PyObject *
py_LM_IsProcessAlive(PyObject *self,
		     PyObject *args)
{
	py_lm_process_obj *pyproc;

	if (!PyArg_ParseTuple(args, "O", &pyproc))
		return NULL;

	if (LM_IsProcessAlive(&pyproc->proc))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

/****************************************/

static PyObject *
py_LM_GetSystemBits(PyObject *self,
		    PyObject *args)
{
	return PyLong_FromSize_t(LM_GetSystemBits());
}

/****************************************/

static lm_bool_t
_py_LM_EnumThreadsCallback(lm_thread_t *pthr,
			   lm_void_t   *arg)
{
	PyObject *pylist = (PyObject *)arg;
	py_lm_thread_obj *pythread;

	pythread = (py_lm_thread_obj *)PyObject_CallObject((PyObject *)&py_lm_thread_t, NULL);
	pythread->thread = *pthr;

	PyList_Append(pylist, (PyObject *)pythread);

	return LM_TRUE;
}

static PyObject *
py_LM_EnumThreads(PyObject *self,
		  PyObject *args)
{
	PyObject *pylist = PyList_New(0);
	if (!pylist)
		return NULL;

	if (!LM_EnumThreads(_py_LM_EnumThreadsCallback, (lm_void_t *)pylist)) {
		Py_DECREF(pylist); /* destroy list */
		pylist = Py_BuildValue("");
	}

	return pylist;
}

/****************************************/

static PyObject *
py_LM_EnumThreadsEx(PyObject *self,
		    PyObject *args)
{
	py_lm_process_obj *pyproc;
	PyObject *pylist;

	if (!PyArg_ParseTuple(args, "O", &pyproc))
		return NULL;

	pylist = PyList_New(0);
	if (!pylist)
		return NULL;

	if (!LM_EnumThreadsEx(&pyproc->proc, _py_LM_EnumThreadsCallback, (lm_void_t *)pylist)) {
		Py_DECREF(pylist); /* destroy list */
		pylist = Py_BuildValue("");
	}

	return pylist;
}

/****************************************/

static PyObject *
py_LM_GetThread(PyObject *self,
		PyObject *args)
{
	lm_thread_t thread;
	py_lm_thread_obj *pythread;

	if (!LM_GetThread(&thread))
		return Py_BuildValue("");

	pythread = (py_lm_thread_obj *)PyObject_CallObject((PyObject *)&py_lm_thread_t, NULL);
	pythread->thread = thread;

	return (PyObject *)pythread;
}

/****************************************/

static PyObject *
py_LM_GetThreadEx(PyObject *self,
		  PyObject *args)
{
	py_lm_process_obj *pyproc;
	lm_thread_t thread;
	py_lm_thread_obj *pythread;

	if (!PyArg_ParseTuple(args, "O", &pyproc))
		return NULL;

	if (!LM_GetThreadEx(&pyproc->proc, &thread))
		return Py_BuildValue("");

	pythread = (py_lm_thread_obj *)PyObject_CallObject((PyObject *)&py_lm_thread_t, NULL);
	pythread->thread = thread;

	return (PyObject *)pythread;
}

/****************************************/

static PyObject *
py_LM_GetThreadProcess(PyObject *self,
		       PyObject *args)
{
	py_lm_thread_obj *pythread;
	lm_process_t proc;
	py_lm_process_obj *pyproc;

	if (!PyArg_ParseTuple(args, "O", &pythread))
		return NULL;

	if (!LM_GetThreadProcess(&pythread->thread, &proc))
		return Py_BuildValue("");

	pyproc = (py_lm_process_obj *)PyObject_CallObject((PyObject *)&py_lm_process_t, NULL);
	pyproc->proc = proc;

	return (PyObject *)pyproc;
}

/****************************************/

static lm_bool_t
_py_LM_EnumModulesCallback(lm_module_t *pmod,
			   lm_void_t   *arg)
{
	PyObject *pylist = (PyObject *)arg;
	py_lm_module_obj *pymodule;

	pymodule = (py_lm_module_obj *)PyObject_CallObject((PyObject *)&py_lm_module_t, NULL);
	pymodule->mod = *pmod;

	PyList_Append(pylist, (PyObject *)pymodule);

	return LM_TRUE;
}

static PyObject *
py_LM_EnumModules(PyObject *self,
		  PyObject *args)
{
	PyObject *pylist = PyList_New(0);
	if (!pylist)
		return NULL;

	if (!LM_EnumModules(_py_LM_EnumModulesCallback, (lm_void_t *)pylist)) {
		Py_DECREF(pylist); /* destroy list */
		pylist = Py_BuildValue("");
	}

	return pylist;
}

/****************************************/

static PyObject *
py_LM_EnumModulesEx(PyObject *self,
		    PyObject *args)
{
	py_lm_process_obj *pyproc;
	PyObject *pylist;

	if (!PyArg_ParseTuple(args, "O", &pyproc))
		return NULL;
       
	pylist = PyList_New(0);
	if (!pylist)
		return NULL;

	if (!LM_EnumModulesEx(&pyproc->proc, _py_LM_EnumModulesCallback, (lm_void_t *)pylist)) {
		Py_DECREF(pylist); /* destroy list */
		pylist = Py_BuildValue("");
	}

	return pylist;
}

/****************************************/

static PyObject *
py_LM_FindModule(PyObject *self,
		 PyObject *args)
{
	lm_char_t        *modstr;
	lm_module_t       mod;
	py_lm_module_obj *pymodule;

#	if LM_CHARSET == LM_CHARSET_UC
	if (!PyArg_ParseTuple(args, "u", &modstr))
			return NULL;
#	else
	if (!PyArg_ParseTuple(args, "s", &modstr))
		return NULL;
#	endif

	if (!LM_FindModule(modstr, &mod))
		return Py_BuildValue("");

	pymodule = (py_lm_module_obj *)PyObject_CallObject((PyObject *)&py_lm_module_t, NULL);
	pymodule->mod = mod;

	return (PyObject *)pymodule;
}

/****************************************/

static PyObject *
py_LM_FindModuleEx(PyObject *self,
		   PyObject *args)
{
	py_lm_process_obj *pyproc;
	lm_char_t         *modstr;
	lm_module_t        mod;
	py_lm_module_obj  *pymodule;

#	if LM_CHARSET == LM_CHARSET_UC
	if (!PyArg_ParseTuple(args, "Ou", &pyproc, &modstr))
			return NULL;
#	else
	if (!PyArg_ParseTuple(args, "Os", &pyproc, &modstr))
		return NULL;
#	endif

	if (!LM_FindModuleEx(&pyproc->proc, modstr, &mod))
		return Py_BuildValue("");

	pymodule = (py_lm_module_obj *)PyObject_CallObject((PyObject *)&py_lm_module_t, NULL);
	pymodule->mod = mod;

	return (PyObject *)pymodule;
}

/****************************************/

static PyObject *
py_LM_LoadModule(PyObject *self,
		 PyObject *args)
{
	lm_char_t        *modpath;
	lm_module_t       mod;
	py_lm_module_obj *pymodule;

#	if LM_CHARSET == LM_CHARSET_UC
	if (!PyArg_ParseTuple(args, "u", &modpath))
			return NULL;
#	else
	if (!PyArg_ParseTuple(args, "s", &modpath))
		return NULL;
#	endif

	if (!LM_LoadModule(modpath, &mod))
		return Py_BuildValue("");

	pymodule = (py_lm_module_obj *)PyObject_CallObject((PyObject *)&py_lm_module_t, NULL);
	pymodule->mod = mod;

	return (PyObject *)pymodule;
}

/****************************************/

static PyObject *
py_LM_LoadModuleEx(PyObject *self,
		   PyObject *args)
{
	py_lm_process_obj *pyproc;
	lm_char_t         *modpath;
	lm_module_t        mod;
	py_lm_module_obj  *pymodule;

#	if LM_CHARSET == LM_CHARSET_UC
	if (!PyArg_ParseTuple(args, "Ou", &pyproc, &modpath))
			return NULL;
#	else
	if (!PyArg_ParseTuple(args, "Os", &pyproc, &modpath))
		return NULL;
#	endif

	if (!LM_LoadModuleEx(&pyproc->proc, modpath, &mod))
		return Py_BuildValue("");

	pymodule = (py_lm_module_obj *)PyObject_CallObject((PyObject *)&py_lm_module_t, NULL);
	pymodule->mod = mod;

	return (PyObject *)pymodule;
}

/****************************************/

static PyObject *
py_LM_UnloadModule(PyObject *self,
		   PyObject *args)
{
	py_lm_module_obj *pymodule;

	if (!PyArg_ParseTuple(args, "O", &pymodule))
		return NULL;

	if (!LM_UnloadModule(&pymodule->mod))
		Py_RETURN_FALSE;

	Py_RETURN_TRUE;
}

/****************************************/

static PyObject *
py_LM_UnloadModuleEx(PyObject *self,
		     PyObject *args)
{
	py_lm_process_obj *pyproc;
	py_lm_module_obj *pymodule;

	if (!PyArg_ParseTuple(args, "OO", &pyproc, &pymodule))
		return NULL;

	if (!LM_UnloadModuleEx(&pyproc->proc, &pymodule->mod))
		Py_RETURN_FALSE;

	Py_RETURN_TRUE;
}

/****************************************/

static lm_bool_t
_py_LM_EnumSymbolsCallback(lm_symbol_t *psym,
			   lm_void_t   *arg)
{
	PyObject *pylist = (PyObject *)arg;
	py_lm_symbol_obj *pysym;

	pysym = (py_lm_symbol_obj *)PyObject_CallObject((PyObject *)&py_lm_symbol_t, NULL);
	pysym->symbol = *psym;
	pysym->name = PyUnicode_FromString(pysym->symbol.name);

	PyList_Append(pylist, (PyObject *)pysym);

	return LM_TRUE;
}

static PyObject *
py_LM_EnumSymbols(PyObject *self,
		  PyObject *args)
{
	py_lm_module_obj *pymodule;
	PyObject *pylist;

	if (!PyArg_ParseTuple(args, "O", &pymodule))
		return NULL;

	pylist = PyList_New(0);
	if (!pylist)
		return NULL;

	if (!LM_EnumSymbols(&pymodule->mod, _py_LM_EnumSymbolsCallback, (lm_void_t *)pylist)) {
		Py_DECREF(pylist); /* destroy list */
		pylist = Py_BuildValue("");
	}

	return pylist;
}

/****************************************/

static PyObject *
py_LM_FindSymbolAddress(PyObject *self,
			PyObject *args)
{
	py_lm_module_obj *pymodule;
	lm_char_t        *symname;
	lm_address_t      symaddr;

#	if LM_CHARSET == LM_CHARSET_UC
	if (!PyArg_ParseTuple(args, "Ou", &pymodule, &symname))
			return NULL;
#	else
	if (!PyArg_ParseTuple(args, "Os", &pymodule, &symname))
		return NULL;
#	endif

	symaddr = LM_FindSymbolAddress(&pymodule->mod, symname);
	if (symaddr == LM_ADDRESS_BAD)
		return Py_BuildValue("");

	return (PyObject *)PyLong_FromSize_t(symaddr);
}

/****************************************/

static PyMethodDef libmem_methods[] = {
	{ "LM_EnumProcesses", py_LM_EnumProcesses, METH_NOARGS, "Lists all current living processes" },
	{ "LM_GetProcess", py_LM_GetProcess, METH_NOARGS, "Gets information about the calling process" },
	{ "LM_GetProcessEx", py_LM_GetProcessEx, METH_VARARGS, "Gets information about a process from a process ID" },
	{ "LM_FindProcess", py_LM_FindProcess, METH_VARARGS, "Searches for an existing process" },
	{ "LM_IsProcessAlive", py_LM_IsProcessAlive, METH_VARARGS, "Checks if a process is alive" },
	{ "LM_GetSystemBits", py_LM_GetSystemBits, METH_VARARGS, "Checks if a process is alive" },
	/****************************************/
	{ "LM_EnumThreads", py_LM_EnumThreads, METH_NOARGS, "Lists all threads from the calling process" },
	{ "LM_EnumThreadsEx", py_LM_EnumThreadsEx, METH_VARARGS, "Lists all threads from the calling process" },
	{ "LM_GetThread", py_LM_GetThread, METH_NOARGS, "Get information about the calling thread" },
	{ "LM_GetThreadEx", py_LM_GetThreadEx, METH_VARARGS, "Get information about a remote thread" },
	{ "LM_GetThreadProcess", py_LM_GetThreadProcess, METH_VARARGS, "Gets information about a process from a thread" },
	/****************************************/
	{ "LM_EnumModules", py_LM_EnumModules, METH_NOARGS, "Lists all modules from the calling process" },
	{ "LM_EnumModulesEx", py_LM_EnumModulesEx, METH_VARARGS, "Lists all modules from a remote process" },
	{ "LM_FindModule", py_LM_FindModule, METH_VARARGS, "Searches for a module in the current process" },
	{ "LM_FindModuleEx", py_LM_FindModuleEx, METH_VARARGS, "Searches for a module in a remote process" },
	{ "LM_LoadModule", py_LM_LoadModule, METH_VARARGS, "Loads a module into the current process" },
	{ "LM_LoadModuleEx", py_LM_LoadModuleEx, METH_VARARGS, "Loads a module into a remote process" },
	{ "LM_UnloadModule", py_LM_UnloadModule, METH_VARARGS, "Unloads a module from the current process" },
	{ "LM_UnloadModuleEx", py_LM_UnloadModuleEx, METH_VARARGS, "Unloads a module from a remote process" },
	/****************************************/
	{ "LM_EnumSymbols", py_LM_EnumSymbols, METH_VARARGS, "Lists all symbols from a module" },
	{ "LM_FindSymbolAddress", py_LM_FindSymbolAddress, METH_VARARGS, "Searches for a symbols in a module" },
	/****************************************/
	{ NULL, NULL, 0, NULL }
};

static PyModuleDef libmem_mod = {
	PyModuleDef_HEAD_INIT,
	"libmem",
	NULL,
	-1,
	libmem_methods
};

PyMODINIT_FUNC
PyInit_libmem(void)
{
	PyObject *pymod;
	PyObject *global; /* used in the DECL_GLOBAL macro */

	if (PyType_Ready(&py_lm_process_t) < 0)
		goto ERR_PYMOD;

	if (PyType_Ready(&py_lm_thread_t) < 0)
		goto ERR_PYMOD;

	if (PyType_Ready(&py_lm_module_t) < 0)
		goto ERR_PYMOD;

	if (PyType_Ready(&py_lm_symbol_t) < 0)
		goto ERR_PYMOD;

	if (PyType_Ready(&py_lm_prot_t) < 0)
		goto ERR_PYMOD;

	if (PyType_Ready(&py_lm_page_t) < 0)
		goto ERR_PYMOD;

	pymod = PyModule_Create(&libmem_mod);
	if (!pymod)
		goto ERR_PYMOD;
	
	/* types */
	Py_INCREF(&py_lm_process_t);
	if (PyModule_AddObject(pymod, "lm_process_t",
			       (PyObject *)&py_lm_process_t) < 0)
		goto ERR_PROCESS;

	Py_INCREF(&py_lm_thread_t);
	if (PyModule_AddObject(pymod, "lm_thread_t",
			       (PyObject *)&py_lm_thread_t) < 0)
		goto ERR_THREAD;

	Py_INCREF(&py_lm_module_t);
	if (PyModule_AddObject(pymod, "lm_module_t",
			       (PyObject *)&py_lm_module_t) < 0)
		goto ERR_MODULE;

	Py_INCREF(&py_lm_symbol_t);
	if (PyModule_AddObject(pymod, "lm_symbol_t",
			       (PyObject *)&py_lm_symbol_t) < 0)
		goto ERR_SYMBOL;

	Py_INCREF(&py_lm_prot_t);
	if (PyModule_AddObject(pymod, "lm_prot_t",
			       (PyObject *)&py_lm_prot_t) < 0)
		goto ERR_PROT;

	Py_INCREF(&py_lm_page_t);
	if (PyModule_AddObject(pymod, "lm_page_t",
			       (PyObject *)&py_lm_page_t) < 0)
		goto ERR_PAGE;

	/* global variables */
	DECL_GLOBAL(LM_PROT_X);
	DECL_GLOBAL(LM_PROT_R);
	DECL_GLOBAL(LM_PROT_W);
	DECL_GLOBAL(LM_PROT_XR);
	DECL_GLOBAL(LM_PROT_XW);
	DECL_GLOBAL(LM_PROT_RW);
	DECL_GLOBAL(LM_PROT_XRW);

	goto EXIT; /* no errors */

ERR_PROT:
	Py_DECREF(&py_lm_prot_t);
	Py_DECREF(pymod);
ERR_PAGE:
	Py_DECREF(&py_lm_page_t);
	Py_DECREF(pymod);
ERR_SYMBOL:
	Py_DECREF(&py_lm_symbol_t);
	Py_DECREF(pymod);
ERR_MODULE:
	Py_DECREF(&py_lm_module_t);
	Py_DECREF(pymod);
ERR_THREAD:
	Py_DECREF(&py_lm_thread_t);
	Py_DECREF(pymod);
ERR_PROCESS:
	Py_DECREF(&py_lm_process_t);
	Py_DECREF(pymod);
ERR_PYMOD:
	pymod = (PyObject *)NULL;
EXIT:
	return pymod;
}

