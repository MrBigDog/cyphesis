// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "rulesets/Fell.h"

#include "rulesets/Entity.h"

#include <Atlas/Objects/Operation/RootOperation.h>

#include <iostream>

#include <cassert>

int main()
{
    int ret = 0;

    Operation op;
    OpVector res;

    {
        Task * task;

        if (0) {
            task->TickOperation(op, res);
        }
    }

    Entity ent1("1"), ent2("2");

    {
        Fell fell(ent1, ent2);

        assert(!fell.obsolete());

        fell.TickOperation(op, res);

        fell.irrelevant();

        assert(fell.obsolete());
    }

    return ret;
}
