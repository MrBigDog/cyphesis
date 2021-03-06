// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "Py_Thing.h"

#include "Py_MemMap.h"
#include "Py_Message.h"
#include "Py_Vector3D.h"
#include "Py_Point3D.h"
#include "Py_Location.h"
#include "Py_Property.h"
#include "Py_Operation.h"
#include "Py_Oplist.h"
#include "Py_Task.h"
#include "Py_WorldTime.h"
#include "PythonWrapper.h"

#include "BaseMind.h"
#include "Character.h"
#include "rulesets/mind/AwareMind.h"

#include "navigation/Steering.h"

#include "common/id.h"
#include "common/TypeNode.h"
#include "common/Inheritance.h"
#include "StatisticsProperty.h"
#include "TerrainProperty.h"
#include "TerrainModProperty.h"
#include "CyPy_Props.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

static PyObject * Entity_as_entity(PyEntity * self)
{
#ifndef NDEBUG
    if (self->m_entity.l == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in Entity.as_entity");
        return nullptr;
    }
#endif // NDEBUG
    PyMessage * ret = newPyMessage();
    if (ret != nullptr) {
        ret->m_obj = new Element(MapType());
        self->m_entity.l->addToMessage(ret->m_obj->asMap());
    }
    return (PyObject *)ret;
}

static PyMethodDef LocatedEntity_methods[] = {
    {"as_entity",       (PyCFunction)Entity_as_entity,  METH_NOARGS},
    {nullptr,              nullptr}           /* sentinel */
};

static PyObject * Entity_send_world(PyEntity * self, PyOperation * op)
{
#ifndef NDEBUG
    if (self->m_entity.e == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in Entity.send_world");
        return nullptr;
    }
#endif // NDEBUG
    if (PyOperation_Check(op)) {
        self->m_entity.e->sendWorld(op->operation);
    } else {
        PyErr_SetString(PyExc_TypeError, "Entity.send_world must be an op");
        return nullptr;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Entity_methods[] = {
    {"send_world",      (PyCFunction)Entity_send_world, METH_O},
    {nullptr,              nullptr}           /* sentinel */
};

static PyObject * Character_start_task(PyEntity * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_entity.l == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in Entity.start_task");
        return nullptr;
    }
#endif // NDEBUG
    PyObject * task;
    PyObject * op;
    PyObject * res;
    if (!PyArg_ParseTuple(args, "OOO", &task, &op, &res)) {
        return nullptr;
    }
    if (!PyTask_Check(task)) {
        PyErr_SetString(PyExc_TypeError, "Entity.start_task must be a task");
        return nullptr;
    }
    if (!PyOperation_Check(op)) {
        PyErr_SetString(PyExc_TypeError, "Entity.start_task must be an op");
        return nullptr;
    }
    if (!PyOplist_Check(res)) {
        PyErr_SetString(PyExc_TypeError, "Entity.start_task must be an oplist");
        return nullptr;
    }
    self->m_entity.c->startTask(((PyTask*)task)->m_task,
                                ((PyOperation*)op)->operation,
                                *((PyOplist*)res)->ops);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * Character_mind2body(PyEntity * self, PyOperation * op)
{
#ifndef NDEBUG
    if (self->m_entity.l == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in Entity.mind2body");
        return nullptr;
    }
#endif // NDEBUG
    if (!PyOperation_Check(op)) {
         PyErr_SetString(PyExc_TypeError, "Entity.mind2body must be an operation");
         return nullptr;
    }
    OpVector res;
    self->m_entity.c->mind2body(op->operation, res);
    if (res.empty()) {
        Py_INCREF(Py_None);
        return Py_None;
    } else if (res.size() == 1) {
        PyOperation * ret = newPyOperation();
        if (ret != nullptr) {
            ret->operation = res[0];
        }
        return (PyObject*)ret;
    } else {
        PyOplist * ret = newPyOplist();
        if (ret != nullptr) {
            ret->ops = new OpVector(res);
        }
        return (PyObject*)ret;
    }
}

static PyObject * Mind_refreshPath(PyEntity * self)
{
#ifndef NDEBUG
    if (self->m_entity.l == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in Entity.refreshPath");
        return nullptr;
    }
#endif // NDEBUG
    AwareMind* awareMind = dynamic_cast<AwareMind*>(self->m_entity.m);
    if (!awareMind) {
        return nullptr;
    }

    int result = awareMind->updatePath();
//    int result = awareMind->getSteering().updatePath(awareMind->m_location.m_pos);
    return Py_BuildValue("i", result);
}

static PyObject * Mind_describeEntity(PyEntity * self)
{
#ifndef NDEBUG
    if (self->m_entity.l == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in Entity.mind2body");
        return nullptr;
    }
#endif // NDEBUG
    AwareMind* awareMind = dynamic_cast<AwareMind*>(self->m_entity.m);
    if (!awareMind) {
        return nullptr;
    }

    std::string result = awareMind->describeEntity();
    return PyUnicode_FromString(result.c_str());
}


static PyObject* Mind_setDestination(PyEntity* self, PyObject* args)
{
    AwareMind* awareMind = dynamic_cast<AwareMind*>(self->m_entity.m);
    if (!awareMind) {
        return nullptr;
    }

    PyObject * destination_arg;
    float radius;
    const char* entityIdString;
    if (!PyArg_ParseTuple(args, "Ofz", &destination_arg, &radius, &entityIdString)) {
        awareMind->getSteering().stopSteering();
        return nullptr;
    }
    if (!PyPoint3D_Check(destination_arg)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a Point3D");
        awareMind->getSteering().stopSteering();
        return nullptr;
    }
    PyPoint3D * destination = (PyPoint3D *)destination_arg;

    if (!destination->coords.isValid()) {
        awareMind->getSteering().stopSteering();
        return 0;
    }

    int entityId;
    //If no entity id was specified, the location is relative to the parent entity.
    if (entityIdString == nullptr) {
        entityId = awareMind->m_location.m_loc->getIntId();
    } else {
        entityId = std::atoi(entityIdString);
    }

    awareMind->getSteering().setDestination(entityId, destination->coords, radius, awareMind->getCurrentServerTime());
    awareMind->getSteering().startSteering();

    Py_INCREF(Py_None);
    return Py_None;

}


static PyMethodDef Character_methods[] = {
    {"start_task",      (PyCFunction)Character_start_task, METH_VARARGS},
    {"mind2body",       (PyCFunction)Character_mind2body,  METH_O},
    {nullptr,              nullptr}           /* sentinel */
};

static PyMethodDef Mind_methods[] = {
    {"refreshPath",     (PyCFunction)Mind_refreshPath,  METH_NOARGS},
    {"setDestination",     (PyCFunction)Mind_setDestination,  METH_VARARGS},
    {"describeEntity",     (PyCFunction)Mind_describeEntity,  METH_NOARGS},
    {nullptr,              nullptr}           /* sentinel */
};

static PyEntity * LocatedEntity_new(PyTypeObject *type,
                                    PyObject *args,
                                    PyObject *kwds)
{
    PyEntity * self = (PyEntity *)type->tp_alloc(type, 0);
    if (self != nullptr) {
        self->m_weakreflist = nullptr;
    }
    return self;
}

static void LocatedEntity_dealloc(PyEntity *self)
{
    if (self->m_weakreflist != nullptr) {
        PyObject_ClearWeakRefs((PyObject *) self);
    }

    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * LocatedEntity_getattro(PyEntity *self, PyObject *oname)
{
#ifndef NDEBUG
    if (self->m_entity.e == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in Entity.getattr");
        return nullptr;
    }
#endif // NDEBUG
    char * name = PyUnicode_AsUTF8(oname);
    // If operation search gets to here, it goes no further

    if (strcmp(name, "id") == 0) {
        return PyUnicode_FromString(self->m_entity.m->getId().c_str());
    }

    if (strcmp(name, "props") == 0) {
        CyPyProps* props = newCyPyProps();
        props->owner = self->m_entity.e;
        self->m_entity.e->incRef();
        Py_INCREF(props);
        return (PyObject*)props;
    }

    if (strcmp(name, "type") == 0) {
        if (self->m_entity.e->getType() == nullptr) {
            PyErr_SetString(PyExc_AttributeError, name);
            return nullptr;
        }
        PyObject * list = PyList_New(0);
        if (list == nullptr) {
            return nullptr;
        }
        PyObject * ent = PyUnicode_FromString(self->m_entity.e->getType()->name().c_str());
        PyList_Append(list, ent);
        Py_DECREF(ent);
        return list;
    }
    if (strcmp(name, "time") == 0) {
        PyWorldTime * worldtime = newPyWorldTime();
        if (worldtime != nullptr) {
            worldtime->time = self->m_entity.m->getTime();
        }
        return (PyObject *)worldtime;
    }
    if (strcmp(name, "location") == 0) {
        PyLocation * loc = newPyLocation();
        if (loc != nullptr) {
            loc->location = &self->m_entity.e->m_location;
            loc->owner = self->m_entity.e;
        }
        return (PyObject *)loc;
    }
    if (strcmp(name, "contains") == 0) {
        if (self->m_entity.e->m_contains == 0) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        PyObject * list = PyList_New(0);
        if (list == nullptr) {
            return nullptr;
        }
        auto I = self->m_entity.e->m_contains->begin();
        auto Iend = self->m_entity.e->m_contains->end();
        for (; I != Iend; ++I) {
            LocatedEntity * child = *I;
            PyObject * wrapper = wrapEntity(child);
            if (wrapper == nullptr) {
                Py_DECREF(list);
                return nullptr;
            }
            PyList_Append(list, wrapper);
            Py_DECREF(wrapper);
        }
        return list;
    }
    if (strcmp(name, "visible") == 0) {
        if (self->m_entity.e->isVisible()) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }

    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int LocatedEntity_setattro(PyEntity *self, PyObject *oname, PyObject *v)
{
#ifndef NDEBUG
    if (self->m_entity.e == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr entity in Entity.setattr");
        return -1;
    }
#endif // NDEBUG
    char * name = PyUnicode_AsUTF8(oname);
    if (strcmp(name, "map") == 0) {
        PyErr_SetString(PyExc_AttributeError, "map attribute forbidden");
        return -1;
    }
    Entity * entity = self->m_entity.e;
    //std::string attr(name);
    //if (v == nullptr) {
        //entity->attributes.erase(attr);
        //return 0;
    //}
    if (strcmp(name, "type") == 0) {
        if (entity->getType() != 0) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot mutate entity type");
            return -1;
        }
        if (!PyUnicode_CheckExact(v)) {
            PyErr_SetString(PyExc_TypeError, "No string type");
            return -1;
        }
        const TypeNode * type = Inheritance::instance().getType(PyUnicode_AsUTF8(v));
        if (type == 0) {
            PyErr_SetString(PyExc_ValueError, "Entity type unknown");
            return -1;
        }
        entity->setType(type);
        return 0;
    }
    auto ret = PyObject_GenericSetAttr((PyObject *)self, oname, v);
    return ret;
}

static PyObject* LocatedEntity_compare(PyObject *a, PyObject *b, int op)
{
    PyObject *result = Py_NotImplemented;

    auto self = (PyEntity*)a;
    if (PyEntity_Check(b)) {
        auto other = (PyEntity*)b;
        if (self->m_entity.e == nullptr || other->m_entity.e == nullptr) {
            result = Py_False;
        }
        if (op == Py_EQ) {
            result = (self->m_entity.e == other->m_entity.e) ? Py_True : Py_False;
        } else if (op == Py_NE) {
            result = (self->m_entity.e != other->m_entity.e) ? Py_True : Py_False;
        }
    } else {
        if (op == Py_EQ) {
            result = Py_False;
        } else if (op == Py_NE) {
            result = Py_True;
        }
    }

    Py_INCREF(result);
    return result;
}


static int LocatedEntity_init(PyEntity * self, PyObject * args, PyObject * kwds)
{
    char * id = nullptr;

    if (!PyArg_ParseTuple(args, "s", &id)) {
        return -1;
    }
    long intId = integerId(id);
    if (intId == -1L) {
        PyErr_SetString(PyExc_TypeError, "Entity() requires string/int ID");
        return -1;
    }
    self->m_entity.l = new Entity(id, intId);
    return 0;
}

static int Entity_init(PyEntity * self, PyObject * args, PyObject * kwds)
{
    PyObject * arg;
    if (!PyArg_ParseTuple(args, "O", &arg)) {
        return -1;
    }
    if (PyUnicode_Check(arg)) {
        char * id = PyUnicode_AsUTF8(arg);
        long intId = integerId(id);
        if (intId == -1L) {
            PyErr_SetString(PyExc_TypeError, "Entity() requires string/int ID");
            return -1;
        }
        self->m_entity.e = new Entity(id, intId);
        return 0;
    }
    if (PyEntity_Check(arg)) {
        PyEntity * character = (PyEntity *)arg;
#ifndef NDEBUG
        if (character->m_entity.c == nullptr) {
            PyErr_SetString(PyExc_AssertionError, "nullptr character Task.__init__");
            return -1;
        }
#endif // NDEBUG
        self->m_entity.c = character->m_entity.c;
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Entity() requires string ID or Entity");
    return -1;
    
}

static int Character_init(PyEntity * self, PyObject * args, PyObject * kwds)
{
    PyObject * arg;
    if (!PyArg_ParseTuple(args, "O", &arg)) {
        return -1;
    }
    if (PyUnicode_Check(arg)) {
        char * id = PyUnicode_AsUTF8(arg);
        long intId = integerId(id);
        if (intId == -1L) {
            PyErr_SetString(PyExc_TypeError, "Entity() requires string/int ID");
            return -1;
        }
        self->m_entity.c = new Character(id, intId);
        return 0;
    }
    if (PyCharacter_Check(arg)) {
        PyEntity * character = (PyEntity *)arg;
#ifndef NDEBUG
        if (character->m_entity.c == nullptr) {
            PyErr_SetString(PyExc_AssertionError, "nullptr character Task.__init__");
            return -1;
        }
#endif // NDEBUG
        self->m_entity.c = character->m_entity.c;
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Character() requires string ID or Character");
    return -1;
    
}

static PyObject * Mind_getattro(PyEntity *self, PyObject *oname)
{
#ifndef NDEBUG
    if (self->m_entity.m == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr mind in Mind.getattr");
        return nullptr;
    }
#endif // NDEBUG
    char * name = PyUnicode_AsUTF8(oname);
    if (strcmp(name, "map") == 0) {
        PyMemMap * map = newPyMemMap();
        if (map != nullptr) {
            map->m_map = self->m_entity.m->getMap();
        }
        return (PyObject *)map;
    }
    if (strcmp(name, "unawareTilesCount") == 0) {
        AwareMind* awareMind = dynamic_cast<AwareMind*>(self->m_entity.m);
        if (!awareMind) {
            return nullptr;
        }

        size_t count = awareMind->getSteering().unawareAreaCount();

        return PyLong_FromSize_t(count);
    }
    if (strcmp(name, "path") == 0) {
        AwareMind* awareMind = dynamic_cast<AwareMind*>(self->m_entity.m);
        if (!awareMind) {
            return nullptr;
        }
        const auto& path = awareMind->getSteering().getPath();
        PyObject * list = PyList_New(0);
        if (list == nullptr) {
            return nullptr;
        }

        for (auto& point : path) {
            auto py_point = newPyPoint3D();
            py_point->coords = point;

            PyList_Append(list, (PyObject*)py_point);
            Py_DECREF(py_point);
        }

        return list;
    }

    if (strcmp(name, "pathResult") == 0) {
        AwareMind* awareMind = dynamic_cast<AwareMind*>(self->m_entity.m);
        if (!awareMind) {
            return nullptr;
        }

        return PyLong_FromLong(awareMind->getSteering().getPathResult());
    }

    return LocatedEntity_getattro(self, oname);
}

static int Mind_setattro(PyEntity *self, PyObject *oname, PyObject *v)
{
    char * name = PyUnicode_AsUTF8(oname);
    if (strcmp(name, "mind") == 0) {
        if (!PyMind_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "Mind.mind requires Mind");
            return -1;
        }
        self->m_entity.m = ((PyEntity*)v)->m_entity.m;
        return 0;
    }
#ifndef NDEBUG
    if (self->m_entity.m == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr mind in Mind.setattr");
        return -1;
    }
#endif // NDEBUG
    if (strcmp(name, "map") == 0) {
        PyErr_SetString(PyExc_AttributeError, "Setting map on mind is forbidden");
        return -1;
    }

    return PyObject_GenericSetAttr((PyObject *)self, oname, v);
}

static int Mind_init(PyEntity * self, PyObject * args, PyObject * kwds)
{
    char * id = nullptr;

    if (!PyArg_ParseTuple(args, "s", &id)) {
        return -1;
    }
    long intId = integerId(id);
    if (intId == -1L) {
        PyErr_SetString(PyExc_TypeError, "Mind() requires string/int ID");
        return -1;
    }
    self->m_entity.m = new BaseMind(id, intId);
    return 0;
}

PyTypeObject PyLocatedEntity_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "server.LocatedEntity",         /*tp_name*/
        sizeof(PyEntity),               /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        (destructor)LocatedEntity_dealloc,     /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,        /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)LocatedEntity_getattro,  // tp_getattro
        (setattrofunc)LocatedEntity_setattro,  // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             // tp_flags
        "LocatedEntity objects",        // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        (richcmpfunc)LocatedEntity_compare,    // tp_richcompare
        offsetof(PyEntity, m_weakreflist), // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        LocatedEntity_methods,          // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        0,                              // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)LocatedEntity_init,   // tp_init
        PyType_GenericAlloc,            // tp_alloc
        (newfunc)LocatedEntity_new,            // tp_new
};

PyTypeObject PyEntity_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "server.Entity",                /*tp_name*/
        sizeof(PyEntity),               /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        0,                              /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             // tp_flags
        "Entity objects",               // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        (richcmpfunc)LocatedEntity_compare,    // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Entity_methods,                 // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        &PyLocatedEntity_Type,          // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Entity_init,          // tp_init
        PyType_GenericAlloc,            // tp_alloc
        (newfunc)LocatedEntity_new,            // tp_new
};

PyTypeObject PyCharacter_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "server.Character",             /*tp_name*/
        sizeof(PyEntity),               /*tp_basicsize*/
        0,                              /*tp_itemsize*/
        /* methods */
        0,                              /*tp_dealloc*/
        0,                              /*tp_print*/
        0,                              /*tp_getattr*/
        0,                              /*tp_setattr*/
        0,                              /*tp_compare*/
        0,                              /*tp_repr*/
        0,                              /*tp_as_number*/
        0,                              /*tp_as_sequence*/
        0,                              /*tp_as_mapping*/
        0,                              /*tp_hash*/
        0,                              // tp_call
        0,                              // tp_str
        0,                              // tp_getattro
        0,                              // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             // tp_flags
        "Character objects",            // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        (richcmpfunc)LocatedEntity_compare,    // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Character_methods,              // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        &PyEntity_Type,                 // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Character_init,       // tp_init
        PyType_GenericAlloc,            // tp_alloc
        (newfunc)LocatedEntity_new,            // tp_new
};

PyTypeObject PyMind_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "server.Mind",                  // tp_name
        sizeof(PyEntity),               // tp_basicsize
        0,                              // tp_itemsize
        // methods 
        0,                              // tp_dealloc
        0,                              // tp_print
        0,                              // tp_getattr
        0,                              // tp_setattr
        0,                              // tp_compare
        0,                              // tp_repr
        0,                              // tp_as_number
        0,                              // tp_as_sequence
        0,                              // tp_as_mapping
        0,                              // tp_hash
        0,                              // tp_call
        0,                              // tp_str
        (getattrofunc)Mind_getattro,    // tp_getattro
        (setattrofunc)Mind_setattro,    // tp_setattro
        0,                              // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             // tp_flags
        "Mind objects",                 // tp_doc
        0,                              // tp_travers
        0,                              // tp_clear
        (richcmpfunc)LocatedEntity_compare,    // tp_richcompare
        0,                              // tp_weaklistoffset
        0,                              // tp_iter
        0,                              // tp_iternext
        Mind_methods,                   // tp_methods
        0,                              // tp_members
        0,                              // tp_getset
        &PyLocatedEntity_Type,          // tp_base
        0,                              // tp_dict
        0,                              // tp_descr_get
        0,                              // tp_descr_set
        0,                              // tp_dictoffset
        (initproc)Mind_init,            // tp_init
        PyType_GenericAlloc,            // tp_alloc
        (newfunc)LocatedEntity_new,            // tp_new
};

template<>
PyObject * wrapPython<LocatedEntity>(LocatedEntity * le)
{
    PyEntity * pe;
    Entity * entity = dynamic_cast<Entity *>(le);
    if (entity != 0) {
        Character * ch_entity = dynamic_cast<Character *>(entity);
        if (ch_entity != 0) {
            pe = newPyCharacter();
            if (pe == nullptr) {
                return nullptr;
            }
            pe->m_entity.c = ch_entity;
        } else {
            pe = newPyEntity();
            if (pe == nullptr) {
                return nullptr;
            }
            pe->m_entity.e = entity;
        }
    } else {
        pe = newPyLocatedEntity();
        if (pe == nullptr) {
            return nullptr;
        }
        pe->m_entity.l = le;
    }
    return (PyObject*)pe;
}

template<>
PyObject * wrapPython<Entity>(Entity * e)
{
    return wrapPython<LocatedEntity>(e);
}

template<>
PyObject * wrapPython<BaseMind>(BaseMind * t)
{
    PyEntity * pm = newPyMind();
    if (pm == nullptr) {
        return nullptr;
    }
    pm->m_entity.m = t;
    return (PyObject*)pm;
}

PyObject * wrapEntity(LocatedEntity * le)
{
    PyObject * wrapper = 0;
    if (le->script() == 0) {
        wrapper = wrapPython(le);
        le->setScript(new PythonWrapper(wrapper));
    } else {
        PythonWrapper * pw = dynamic_cast<PythonWrapper *>(le->script());
        if (pw == 0) {
            log(WARNING, "Entity has script of unknown type");
        } else {
            wrapper = pw->wrapper();
            assert(wrapper != nullptr);
            Py_INCREF(wrapper);
        }
    }
    return wrapper;
}

// FIXME if this is cleaner to use the methods field of the type for methods,
// one PyTypeObject for each, and just share the members. That way we lose
// the need for a members entry on the object.

PyEntity * newPyLocatedEntity()
{
    return (PyEntity *)PyLocatedEntity_Type.tp_new(&PyLocatedEntity_Type, 0, 0);
}

PyEntity * newPyEntity()
{
    return (PyEntity *)PyEntity_Type.tp_new(&PyEntity_Type, 0, 0);
}

PyEntity * newPyCharacter()
{
    return (PyEntity *)PyCharacter_Type.tp_new(&PyCharacter_Type, 0, 0);
}

PyEntity * newPyMind()
{
    return (PyEntity *)PyMind_Type.tp_new(&PyMind_Type, 0, 0);
}
