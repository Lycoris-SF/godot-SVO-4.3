#ifndef SVO_NAVMAP_MANAGER_H
#define SVO_NAVMAP_MANAGER_H

#include "svo_navmap.h"
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_set.hpp>
//#include <godot_cpp/templates/hash_map.hpp>

#include <queue>

namespace godot {

    class SvoNavmapManager : public Node {
        GDCLASS(SvoNavmapManager, Node);
    private:
        Dictionary navmeshes;

        void acquire_navmesh(SvoNavmap* navmesh);
        bool query(Vector3 position) const;

        Vector<Vector3> find_path(Vector3 start, Vector3 end) const;
        SvoNavmap* find_navmesh_containing(Vector3 position) const;

        void tidy_adjacents();
        void bfs(String start_uuid, HashSet<String>& visited);
        Array find_possible_roots();

    public:
        SvoNavmapManager();
        ~SvoNavmapManager();

    protected:
        static void _bind_methods();
    };

}

#endif // SVO_NAVMESH_MANAGER_H