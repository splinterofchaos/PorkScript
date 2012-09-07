/********************************************************
PorkScript Game Engine (C) 2012 Andy J. Brennan
All Rights Reserved.

PorkPy.h -- Header file that defines the PorkPy Python
Interpreter Encapsulator.
********************************************************/
#ifndef _PORKPY_H
#define _PORKPY_H

extern "C"
{
	#include <Python.h>
	#include <structmember.h>
}

#include <PGameContainer.h>

#pragma warning(push)
#pragma warning(disable : 4482)

using GameContainer::GlobalTypeEnum;
using GameContainer::PGameContainer;
using GameContainer::PMap;
using GameContainer::PItem;

PGameContainer *glContainer;
std::list<PItem> *inventory;

void SetGC(PGameContainer *GC, std::list<PItem> *inv)
{
	if(GC)
		glContainer = GC;

	if(inv)
		inventory = inv;

	return;
}

extern "C"
{
	//****************************************************
	//*
	//*  Module Types
	//*
	//****************************************************

	// Definition for a map
	typedef struct
	{
		PyObject_HEAD
		PyObject *ID;
		PyObject *name;
		PyObject *desc;
		PyObject *items;
	} Map;

	static void Map_dealloc(Map* self)
	{
		Py_XDECREF(self->ID);
		Py_XDECREF(self->name);
		Py_XDECREF(self->desc);
		Py_XDECREF(self->items);
		self->ob_type->tp_free((PyObject*)self);
	}

	static PyObject *Map_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
	{
		Map *self;

		self = (Map *)type->tp_alloc(type,0);
		if(self != NULL)
		{
			self->ID = PyString_FromString("");
			if(self->ID == NULL)
			{
				Py_DECREF(self);
				return NULL;
			}
			
			self->name = PyString_FromString("");
			if(self->name == NULL)
			{
				Py_DECREF(self);
				return NULL;
			}

			self->desc = PyString_FromString("");
			if(self->desc == NULL)
			{
				Py_DECREF(self);
				return NULL;
			}

			self->items = PyDict_New();
			if(self->items == NULL)
			{
				Py_DECREF(self);
				return NULL;
			}
		}

		return (PyObject *)self;
	}

	static int Map_init(Map *self, PyObject *args, PyObject *kwds)
	{
		PyObject *ID=NULL, *name=NULL, *desc=NULL, *items=NULL, *tmp;

		static char *kwlist[] = {"ID",NULL};

		if(!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &ID))
			return -1;

		if(ID)
		{
			tmp = self->ID;
			Py_INCREF(ID);
			self->ID = ID;
			Py_XDECREF(tmp);
		}

		PMap *theMap = glContainer->GetPMap(PyString_AsString(self->ID));
		
		if(theMap)
		{
			name = PyString_FromString(theMap->GetName().c_str());
			tmp = self->name;
			Py_INCREF(name);
			self->name = name;
			Py_XDECREF(tmp);

			desc = PyString_FromString(theMap->GetDesc().c_str());
			tmp = self->desc;
			Py_INCREF(desc);
			self->desc = desc;
			Py_XDECREF(tmp);


			map<string,UINT> iList = theMap->ListItems();
			map<string,UINT>::iterator it = iList.begin();
			items = PyDict_New();
			tmp = self->items;
			Py_INCREF(items);
			for(UINT i=0;i<iList.size();i++,it++)
			{ 
				if(PyDict_SetItemString(items, it->first.c_str(), PyInt_FromSize_t(it->second)) < 0)
					return -1;
			}
			self->items = items;
			Py_XDECREF(tmp);
		}
		else
			return -1;

		return 0;
	}

	static PyMemberDef Map_members[] = {
		{"ID",T_OBJECT_EX, offsetof(Map, ID), 0, "Map ID"},
		{"name",T_OBJECT_EX, offsetof(Map, name), 0, "Map Name"},
		{"desc",T_OBJECT_EX, offsetof(Map, desc), 0, "Map Description"},
		{"items",T_OBJECT_EX, offsetof(Map, items), 0, "Map ItemList"},
		{NULL} // Sentinel
	};

	static PyTypeObject MapType = {
		PyObject_HEAD_INIT(NULL)
		0,                         /*ob_size*/
		"PorkPy.Map",             /*tp_name*/
		sizeof(Map),             /*tp_basicsize*/
		0,                         /*tp_itemsize*/
		(destructor)Map_dealloc, /*tp_dealloc*/
		0,                         /*tp_print*/
		0,                         /*tp_getattr*/
		0,                         /*tp_setattr*/
		0,                         /*tp_compare*/
		0,                         /*tp_repr*/
		0,                         /*tp_as_number*/
		0,                         /*tp_as_sequence*/
		0,                         /*tp_as_mapping*/
		0,                         /*tp_hash */
		0,                         /*tp_call*/
		0,                         /*tp_str*/
		0,                         /*tp_getattro*/
		0,                         /*tp_setattro*/
		0,                         /*tp_as_buffer*/
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
		"PorkScript Map Object",           /* tp_doc */
		0,		               /* tp_traverse */
		0,		               /* tp_clear */
		0,		               /* tp_richcompare */
		0,		               /* tp_weaklistoffset */
		0,		               /* tp_iter */
		0,		               /* tp_iternext */
		0,             /* tp_methods */
		Map_members,             /* tp_members */
		0,                         /* tp_getset */
		0,                         /* tp_base */
		0,                         /* tp_dict */
		0,                         /* tp_descr_get */
		0,                         /* tp_descr_set */
		0,                         /* tp_dictoffset */
		(initproc)Map_init,      /* tp_init */
		0,                         /* tp_alloc */
		Map_new,                 /* tp_new */
	};

	// Definition for an Item
	typedef struct
	{
		PyObject_HEAD
		PyObject *ID;
		PyObject *name;
		PyObject *desc;
	} Item;

	static void Item_dealloc(Item* self)
	{
		Py_XDECREF(self->ID);
		Py_XDECREF(self->name);
		Py_XDECREF(self->desc);
		self->ob_type->tp_free((PyObject*)self);
	}

	static PyObject *Item_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
	{
		Item *self;

		self = (Item *)type->tp_alloc(type,0);
		if(self != NULL)
		{
			self->ID = PyString_FromString("");
			if(self->ID == NULL)
			{
				Py_DECREF(self);
				return NULL;
			}
			
			self->name = PyString_FromString("");
			if(self->name == NULL)
			{
				Py_DECREF(self);
				return NULL;
			}

			self->desc = PyString_FromString("");
			if(self->desc == NULL)
			{
				Py_DECREF(self);
				return NULL;
			}
		}

		return (PyObject *)self;
	}

	static int Item_init(Item *self, PyObject *args, PyObject *kwds)
	{
		PyObject *ID=NULL, *name=NULL, *desc=NULL, *tmp;

		static char *kwlist[] = {"ID",NULL};

		if(!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &ID))
			return -1;

		if(ID)
		{
			tmp = self->ID;
			Py_INCREF(ID);
			self->ID = ID;
			Py_XDECREF(tmp);
		}

		PItem *theItem = glContainer->GetPItem(PyString_AsString(self->ID));
		
		if(theItem)
		{
			name = PyString_FromString(theItem->GetName().c_str());
			tmp = self->name;
			Py_INCREF(name);
			self->name = name;
			Py_XDECREF(tmp);

			desc = PyString_FromString(theItem->GetDesc().c_str());
			tmp = self->desc;
			Py_INCREF(desc);
			self->desc = desc;
			Py_XDECREF(tmp);
		}
		else
			return -1;

		return 0;
	}

	static PyMemberDef Item_members[] = {
		{"ID",T_OBJECT_EX, offsetof(Item, ID), 0, "Item ID"},
		{"name",T_OBJECT_EX, offsetof(Item, name), 0, "Item Name"},
		{"desc",T_OBJECT_EX, offsetof(Item, desc), 0, "Item Description"},
		{NULL} // Sentinel
	};

	static PyTypeObject ItemType = {
		PyObject_HEAD_INIT(NULL)
		0,                         /*ob_size*/
		"PorkPy.Item",             /*tp_name*/
		sizeof(Item),             /*tp_basicsize*/
		0,                         /*tp_itemsize*/
		(destructor)Item_dealloc, /*tp_dealloc*/
		0,                         /*tp_print*/
		0,                         /*tp_getattr*/
		0,                         /*tp_setattr*/
		0,                         /*tp_compare*/
		0,                         /*tp_repr*/
		0,                         /*tp_as_number*/
		0,                         /*tp_as_sequence*/
		0,                         /*tp_as_mapping*/
		0,                         /*tp_hash */
		0,                         /*tp_call*/
		0,                         /*tp_str*/
		0,                         /*tp_getattro*/
		0,                         /*tp_setattro*/
		0,                         /*tp_as_buffer*/
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
		"PorkScript Item Object",           /* tp_doc */
		0,		               /* tp_traverse */
		0,		               /* tp_clear */
		0,		               /* tp_richcompare */
		0,		               /* tp_weaklistoffset */
		0,		               /* tp_iter */
		0,		               /* tp_iternext */
		0,					/* tp_methods */
		Item_members,             /* tp_members */
		0,                         /* tp_getset */
		0,                         /* tp_base */
		0,                         /* tp_dict */
		0,                         /* tp_descr_get */
		0,                         /* tp_descr_set */
		0,                         /* tp_dictoffset */
		(initproc)Item_init,      /* tp_init */
		0,                         /* tp_alloc */
		Item_new,                 /* tp_new */
	};

	//****************************************************
	//*
	//*  Module Methods
	//*
	//****************************************************
	static PyObject* PorkPy_GetAttr(PyObject* self, PyObject *args)
	{
		const char *id;
		const char *attr;
		const char *module;

		if(!PyArg_ParseTuple(args, "sss", &id, &attr, &module))
			return NULL;
		
		if(!glContainer)
			return NULL;
		
		GlobalTypeEnum type = glContainer->GetTypeByID(id);

		if(type == GlobalTypeEnum::ITEM)
		{
			if(!glContainer->GetPItem(id)->PropertyExist(module, attr))
				return NULL;
			GlobalTypeEnum attrType = glContainer->GetPItem(id)->GetPropType(module, attr);

			if(attrType == GlobalTypeEnum::NONETYPE)
				return NULL;
			else if(attrType == GlobalTypeEnum::ITEM || attrType == GlobalTypeEnum::MAP || attrType == GlobalTypeEnum::TEXT)
				return Py_BuildValue("(i,s)", attrType, glContainer->GetPItem(id)->GetPropAsString(attr, module));
			else if(attrType == GlobalTypeEnum::NUM)
				return Py_BuildValue("(i,l)", attrType, glContainer->GetPItem(id)->GetPropAsLong(attr, module));
			else if(attrType == GlobalTypeEnum::BOOL)
				return Py_BuildValue("(i,s)", attrType, glContainer->GetPItem(id)->GetPropAsString(attr, module));
			else
				return NULL;
		}
		else if(type == GlobalTypeEnum::MAP)
		{
			if(!glContainer->GetPMap(id)->PropertyExist(module, attr))
				return NULL;
			GlobalTypeEnum attrType = glContainer->GetPMap(id)->GetPropType(module, attr);

			if(attrType == GlobalTypeEnum::NONETYPE)
				return NULL;
			else if(attrType == GlobalTypeEnum::ITEM || attrType == GlobalTypeEnum::MAP || attrType == GlobalTypeEnum::TEXT)
				return Py_BuildValue("(i,s)", attrType, glContainer->GetPMap(id)->GetPropAsString(attr, module));
			else if(attrType == GlobalTypeEnum::NUM)
				return Py_BuildValue("(i,l)", attrType, glContainer->GetPMap(id)->GetPropAsLong(attr, module));
			else if(attrType == GlobalTypeEnum::BOOL)
				return Py_BuildValue("(i,s)", attrType, glContainer->GetPMap(id)->GetPropAsString(attr, module));
			else
				return NULL;
		}
		else
			return NULL;
	}

	static PyObject* PorkPy_GetDyn(PyObject* self, PyObject* args)
	{
		const char *id;
		const char *attr;
		const char *module;

		if(!PyArg_ParseTuple(args, "sss", &id, &attr, &module))
			return NULL;

		if(!glContainer)
			return NULL;

		GlobalTypeEnum type = glContainer->GetTypeByID(id);

		if(type == GlobalTypeEnum::ITEM)
		{
			if(!glContainer->GetPItem(id)->PropertyExist(module, attr))
				return NULL;
			GlobalTypeEnum attrType = glContainer->GetPItem(id)->GetPropType(module, attr);

			if(attrType == GlobalTypeEnum::NONETYPE)
				return NULL;
			else if(attrType == GlobalTypeEnum::ITEM || attrType == GlobalTypeEnum::MAP || attrType == GlobalTypeEnum::TEXT)
				return Py_BuildValue("(i,s)", attrType, glContainer->GetPItem(id)->GetPropAsString(attr, module));
			else if(attrType == GlobalTypeEnum::NUM)
				return Py_BuildValue("(i,l)", attrType, glContainer->GetPItem(id)->GetPropAsLong(attr, module));
			else if(attrType == GlobalTypeEnum::BOOL)
				return Py_BuildValue("(i,s)", attrType, glContainer->GetPItem(id)->GetPropAsString(attr, module));
			else
				return NULL;
		}
		else if(type == GlobalTypeEnum::MAP)
		{
			if(!glContainer->GetPMap(id)->PropertyExist(module, attr))
				return NULL;
			GlobalTypeEnum attrType = glContainer->GetPMap(id)->GetPropType(module, attr);

			if(attrType == GlobalTypeEnum::NONETYPE)
				return NULL;
			else if(attrType == GlobalTypeEnum::ITEM || attrType == GlobalTypeEnum::MAP || attrType == GlobalTypeEnum::TEXT)
				return Py_BuildValue("(i,s)", attrType, glContainer->GetPMap(id)->GetPropAsString(attr, module));
			else if(attrType == GlobalTypeEnum::NUM)
				return Py_BuildValue("(i,l)", attrType, glContainer->GetPMap(id)->GetPropAsLong(attr, module));
			else if(attrType == GlobalTypeEnum::BOOL)
				return Py_BuildValue("(i,s)", attrType, glContainer->GetPMap(id)->GetPropAsString(attr, module));
			else
				return NULL;
		}
		else
			return NULL;
	}
	
	static PyMethodDef PorkPy_methods[] = {
		{"GetAttr", PorkPy_GetAttr, METH_VARARGS,
		"Get an attribute from an entity."},

		{"GetDyn", PorkPy_GetDyn, METH_VARARGS,
		"Get a dynamic version of an attribute."},

		{NULL,NULL,0,NULL} // Sentinel
	};

	#ifndef PyMODINIT_FUNC
	#define PyMODINIT_FUNC void
	#endif

	PyMODINIT_FUNC initPorkPy(void)
	{
		PyObject* m;

		if(PyType_Ready(&MapType) < 0)
			return;
		
		if(PyType_Ready(&ItemType) < 0)
			return;

		m = Py_InitModule3("PorkPy", PorkPy_methods, "PorkPy PorkScript->Python Bridge.");

		if(m == NULL)
			return;

		Py_INCREF(&MapType);
		PyModule_AddObject(m, "Map", (PyObject*)&MapType);

		Py_INCREF(&ItemType);
		PyModule_AddObject(m, "Item", (PyObject*)&ItemType);
	}

	// PorkPy.cpp function prototypes
	void RunCmd(string module, string args);
	void CompileAndGetProps(string module);
}



#pragma warning(pop)

#endif // _PORKPY_H