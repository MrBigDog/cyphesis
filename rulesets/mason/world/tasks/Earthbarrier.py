#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *

import math
import weakref

import server

class Earthbarrier(server.Task):
    """ A task for creating Earthbarrier with a shovel."""
    class Obstructed(Exception):
        "An exception indicating this task is obstructed by an entity>"
        pass

    materials = { 0: 'boulder', 1: 'sand', 2: 'earth', 3: 'silt', 4: 'ice' }
    def walls_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Earthbarrier.walls"

        if len(op) < 1:
            sys.stderr.write("Earthbarrier task has no target in Earthbarrier op")

        self.target = server.world.get_object_ref(op[0].id)
        self.tool = op.to

        self.pos = Point3D(op[0].pos)

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Earthbarrier.tick"

        if self.target() is None:
            # print "Target is no more"
            self.irrelevant()
            return


        old_rate = self.rate

        self.rate = 0.5 / 0.75
        self.progress += 0.5

        if old_rate < 0.01:
            self.progress = 0

        # print "%s" % self.pos

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        self.progress = 0


        res=Oplist()


        if not hasattr(self, 'terrain_mod') or self.terrain_mod() is None:
            try:
                mod = self._find_mod('wall')
            except Obstructed:
                self.irrelevant()
                return
            if mod is None:
                # There is no terrain mod where we are digging,
                return self._create_initial_mod()
            print "found existing mod"
            self.terrain_mod = weakref.ref(mod)
        else: pass

        mod = self.terrain_mod()
        print "Now we grow it"
        area = mod.terrainmod.shape.area()
        factor = math.sqrt((area + 1) / area)
        # FIXME Shape is not compatible with line data yet
        line = Shape(mod.line)
        mod.terrainmod.shape *= factor
        mod.terrainmod.height += 1 / area
        box = mod.terrainmod.shape.footprint()
        mod.bbox = [box.low_corner().x,
                    box.low_corner().y,
                    0,
                    box.high_corner().x,
                    box.high_corner().y,
                    mod.terrainmod.height]

        area_map = {'shape': mod.terrainmod.shape.as_data(),
                    'layer': 7}
        # FIXME This area is not getting broadcast
        mod.area = area_map
        # We have modified the attribute in place,
        # so must send an update op to propagate
        res.append(Operation("update", to=mod.id))


        res.append(self.next_tick(0.75))

        return res
    def _find_mod(self, name):
        mods = self.target().terrain.find_mods(self.pos)
        if len(mods) != 0:
            for mod in mods:
                if hasattr(mod, 'name') and mod.name == name:
                    return mod
            raise Obstructed, "Another mod is in the way"
    def _create_initial_mod(self):
        # There is no terrain mod where we are making wall,
        # so we check if it is in the materials , and if so create
        # a wall
        surface = self.target().terrain.get_surface(self.pos)
        # print "SURFACE %d at %s" % (surface, self.pos)
        if surface not in Earthbarrier.materials:
            # print "Not in material"
            self.irrelevant()
            return
        self.surface = surface

        z=self.character.location.coordinates.z + 1.0
        modmap = {
                  'height': z,
                  'shape': {
                            'points': [[ -1.0, -1.0 ],
                                       [ -1.0, 1.0 ],
                                       [ 1.0, 1.0 ],
                                       [ 1.0, -1.0 ]],
                            'type': 'polygon'
                           },
                  'type': 'levelmod'
                  }
        area_map = {'shape': modmap['shape'],
                    'layer': 7}
        line_map = {'points': [[ self.pos.x, self.pos.y ]],
                    'type': 'line'}

        wall_loc = Location(self.character.location.parent)
        wall_loc.velocity = Vector3D()
        wall_loc.coordinates = self.pos

        walls_create=Operation("create",
                                Entity(name="wall",
                                       type="path",
                                       line = line_map,
                                       location = wall_loc,
                                       terrainmod = modmap,
                                       area = area_map),
                                to=self.target())
        res=Oplist()
        res.append(walls_create)
        res.append(self.next_tick(0.75))
        return res
