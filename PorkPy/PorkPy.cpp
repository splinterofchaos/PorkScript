/********************************************************
PorkScript Game Engine (C) 2012 Andy J. Brennan
All Rights Reserved.

PorkPy.cpp -- Main source file for PorkPy, including
RunCmd(), the function that calls our embedded Python.
********************************************************/

#include "PorkPy.h"
#include "PorkPyExceptions.h"

extern "C"
{
	// Import script to compile, and then get custom properties
	void CompileAndGetProps(string module)
	{
		PyObject *pCmdMod, *pDict, *pCmdClass, *pCmdArgs;
		Py_Initialize();

		// Import the module (ex. kill.py)
		pCmdMod = PyImport_ImportModule(module.c_str());
		if(!pCmdMod)
			throw xException("Failed to import module '"+module+"'!");

		// Get all properties for this module


		Py_DECREF(pCmdMod); // Done with module.
		Py_Finalize(); // Done with embedded python.

		return;
	}

	// Run a Pork command after starting interpreter
	void RunCmd(string module, string args)
	{
		PyObject *pCmdMod, *pCmdClassName, *pDict, *pCmdClass, *pCmdArgs;
		Py_Initialize();

		// Import the module (ex. kill.py)
		pCmdMod = PyImport_ImportModule(module.c_str());
		if(!pCmdMod)
			throw xException("Failed to import module '"+module+"'!");

		pDict = PyDict_New();

		// Build the class name from module
		pCmdClassName = PyDict_GetItemString(pDict, module.c_str());
		Py_XDECREF(pDict);

		// Call the command class
		if(PyCallable_Check(pCmdClassName))
		{
			pCmdArgs = Py_BuildValue("(s)",args.c_str());
			pCmdClass = PyObject_CallObject(pCmdClassName, pCmdArgs);
			Py_DECREF(pCmdArgs);
			Py_DECREF(pCmdClass);
			Py_DECREF(pCmdClassName);
		}
		else
		{
			Py_XDECREF(pCmdClassName);
			Py_XDECREF(pCmdMod);
			throw xException("Failed to get class '"+module+"'!");
		}

		// Once called, all neccassary callback functions
		// have been called, so we are effectively done.
		// So now time for cleanup.
		Py_DECREF(pCmdMod); // Done with module.
		Py_Finalize(); // Done with embedded python.

		return;
	}
}