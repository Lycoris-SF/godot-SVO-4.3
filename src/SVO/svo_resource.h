#ifndef SVO_RESOURCES_H
#define SVO_RESOURCES_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>

#include "svo_navmap.h"

namespace godot {
    class SparseVoxelOctreeResource : public Resource {
        GDCLASS(SparseVoxelOctreeResource, Resource)

    protected:
        static void _bind_methods() {
            // 绑定属性和方法
        }

    public:
        SvoNavmap *svo;
        void save();
        void read();

        SparseVoxelOctreeResource();
    };
}

#endif // SVO_RESOURCES_H