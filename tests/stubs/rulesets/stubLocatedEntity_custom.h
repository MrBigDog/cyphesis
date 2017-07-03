//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#ifndef STUB_LocatedEntity_setType
#define STUB_LocatedEntity_setType

void LocatedEntity::setType(const TypeNode* t)
{
    m_type = t;
}

#endif

#ifndef STUB_LocatedEntity_makeContainer
#define STUB_LocatedEntity_makeContainer

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

#endif