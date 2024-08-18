#ifndef SVO_STRUCTURE_H
#define SVO_STRUCTURE_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/box_mesh.hpp>

namespace godot {

    enum VoxelState {
        VS_EMPTY,
        VS_SOLID,
        VS_LIQUID
    };

    class Voxel : public Node {
        GDCLASS(Voxel, Node);
    public:
        VoxelState state;
        float size;
        Voxel();
        Voxel(VoxelState state, float voxel_size);
        bool isSolid();
        bool isEmpty();
        bool isLiquid();
        String to_string() const;
    private:
        String state_to_string(VoxelState state) const;
    protected:
        static void _bind_methods();
    };

    class OctreeNode : public Node {
        GDCLASS(OctreeNode, Node);

    public:
        bool isLeaf;
        int currentDepth;
        int currentIndex;
        Vector3 center;
        Vector3 centerGlobal;   //_process this if svo is dynamic
        Voxel* voxel;
        OctreeNode* father;
        OctreeNode* children[8];
        OctreeNode* neighbors[6];

        // debug draw
        MeshInstance3D* debugMesh;
        Ref<BoxMesh> debugBoxMesh;
        bool debugChecked;

        // debug print
        String get_voxel_info() const;
        void setDebugMesh(MeshInstance3D* mesh);
        void freeDebugMesh();
        bool isDebugMeshValid() const;

        OctreeNode();
        OctreeNode(OctreeNode* father_node, int depth = -1, int index = -1);
        ~OctreeNode();

    private:

        void queue_free_debug_mesh();

    protected:
        static void _bind_methods();
    };

    class SparseVoxelOctree : public Node {
        GDCLASS(SparseVoxelOctree, Node);
    public:
        int maxDepth;
        float voxelSize;
        Transform3D last_transform;
        OctreeNode* root;

        SparseVoxelOctree();
        SparseVoxelOctree(int max_depth, float voxelSize);
        ~SparseVoxelOctree();

        void insert(Vector3 pos);
        bool query(Vector3 pos) const;
        OctreeNode* get_deepest_node(Vector3 pos);
        void update(Vector3 pos, bool isSolid);
        void update_node_centers();
        void update_global_centers();
        void compress_node();
        void expand_node();
        void clear();

        // other tool
        float calActualVoxelSize(int depth);
        void create_empty_children(OctreeNode* node, int depth);
        void create_solid_children(OctreeNode* node, int depth);
        void evaluate_homogeneity(OctreeNode* node);

    private:
        void insert(OctreeNode* node, Vector3 pos, Vector3 center, int depth);
        bool query(OctreeNode* node, Vector3 pos) const;
        void update(OctreeNode* node, Vector3 pos, Vector3 center, int depth, VoxelState newState);
        // functional update
        void update_node_centers(OctreeNode* node, Vector3 center, int depth);
        void update_global_centers(OctreeNode* node);
        void compress_node(OctreeNode* node, int depth);
        void expand_node(OctreeNode* node, int depth);
        void reevaluate_homogeneity_insert(OctreeNode* node);
        void reevaluate_homogeneity_update(OctreeNode* node);
        // delete
        void deleteChildren(OctreeNode* node);
        // other tool
        void merge_children(OctreeNode* node);

    protected:
        static void _bind_methods();
    };

}

#endif // SVO_STRUCTURE_H