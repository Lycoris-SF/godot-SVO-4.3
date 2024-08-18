#ifndef SVO_NAVMAP_H
#define SVO_NAVMAP_H

// variant & container
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

// shape
#include <godot_cpp/classes/physics_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/box_shape3d.hpp>
#include <godot_cpp/classes/cylinder_shape3d.hpp>
#include <godot_cpp/classes/cylinder_mesh.hpp>
#include <godot_cpp/classes/sphere_mesh.hpp>

// physics
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_point_query_parameters3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>
#include <godot_cpp/classes/physics_shape_query_parameters3d.hpp>
#include <godot_cpp/classes/world3d.hpp>

// Material
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>

/*// Multi thread
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/semaphore.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/core/binder_common.hpp>*/

#include "svo_structure.h"
#include "svo_connector.h"
#include "Test/test_svo.h"

namespace godot {
    static Ref<StandardMaterial3D> debugSolidMaterial;
    static Ref<StandardMaterial3D> debugCheckMaterial;
    static Ref<ShaderMaterial> debugEmptyMaterial;
    static Ref<StandardMaterial3D> debugPathMaterialB;
    static Ref<StandardMaterial3D> debugPathMaterialY;
    static Ref<StandardMaterial3D> debugPathMaterialR;
    static Ref<Shader> EmptyMaterial_shader;
    // PhysicsBody3Ds' RID
    static Vector<RID> target_rids;
    //
    static int sum_time = 0;
    static int sum_count = 0;

    class SvoNavmap : public Node3D {
        GDCLASS(SvoNavmap, Node3D)

    private:
        SparseVoxelOctree *svo;
        Vector<SvoConnector*> outer_connector;
        Vector<SvoConnector*> inner_connector;

        int maxDepth;
        float rootVoxelSize;    // size of the root cube
        float minVoxelSize;     // size of the smallest cube
        double testdouble;
        int collision_layer;    // Manual sync required

        // <debug draw>
        bool debug_mode;
        bool node_ready;
        int DrawRef_minDepth;
        int DrawRef_maxDepth;
        bool show_empty;
        bool show_path;
        float debug_path_scale;
        bool get_debug_mode() const;
        void set_debug_mode(bool debug_mode);
        int get_DR_min_depth() const;
        void set_DR_min_depth(int depth);
        int get_DR_max_depth() const;
        void set_DR_max_depth(int depth);
        bool get_show_empty() const;
        void set_show_empty(bool show_empty);
        bool get_show_path() const;
        void set_show_path(bool show_path);
        int get_collision_layer() const;
        void set_collision_layer(int layer);
        float get_debug_path_scale() const;
        void set_debug_path_scale(float scale);
        void manual_set_node_ready();

        OctreeNode* debugChecked_node;

        // v3
        Vector<OctreeNode*> exist_meshes;   // Node of MeshInstance3D children in SvoNavmap: Node3D
        Vector<OctreeNode*> active_meshes;  // Node of active MeshInstance list
        
        // debug path
        Vector<Vector3> exist_path;
        Vector<MeshInstance3D*> path_pool;

        // v3
        void init_debug_mesh_v3();
        void init_debug_mesh_v3(OctreeNode* node, int depth);
        void reset_pool_v3();
        void draw_svo_v3(OctreeNode* node, int current_depth, int min_depth, int max_depth);
        MeshInstance3D* get_mesh_instance_from_pool();

        // debug path
        void reset_debugCheck();
        void init_debug_path(float agent_r);
        void draw_path_switch_visible(bool show);
        // </debug draw>

        // structure
        void insert_svo_based_on_collision_shapes();
        bool check_point_inside_mesh(Vector3 point);
        bool check_box_intersect_mesh(Vector3 position, Quaternion rotation, float size);
        bool is_box_fully_inside_mesh(Vector3 position, float size);
        void collect_collision_shapes(Node* node);
        void traverse_svo_space_and_insert(OctreeNode* node, int depth);
        Vector<Vector3> calculate_connector_points();

        // neighbors
        void init_neighbors();
        void set_neighbors(OctreeNode* node);
        void set_neighbors_from_brother(OctreeNode* node);

        // path finding
        bool can_travel_directly_with_cylinder(const Vector3& from, const Vector3& to, float agent_radius);
        bool can_travel_directly_with_ray(const Vector3& from, const Vector3& to);
        Vector<Vector3> subdivide_path(Vector3 start, Vector3 end, float segment_length);
        // jump
        //OctreeNode* jump(Vector<OctreeNode*>& open_set, const OctreeNode* current, const Vector3& direction, const OctreeNode* end_node);

        // tools
        Vector3 worldToGrid(Vector3 world_position);
        Vector3 gridToWorld(Vector3 grid_position);

    protected:
        static void _bind_methods();

    public:
        SvoNavmap();
        //SvoNavmap(float size, Vector3 position, Vector3 rotation);
        ~SvoNavmap();

        // uuid
        String uuid;            // need support from minos-UUID-generator-for-godot
        Array adjacent_uuids;
        void set_uuid(String p_uuid);
        String get_uuid() const;
        void set_adjacent_list(Array data);
        Array get_adjacent_list() const;

        // plugin
        void insert_voxel(Vector3 position);
        bool query_voxel(Vector3 position);
        void check_voxel_with_id(String id);
        void update_voxel(Vector3 position, bool isSolid);

        //svo setting
        float get_voxel_size() const;
        void set_voxel_size(float size);
        int get_max_depth() const;
        void set_max_depth(int depth);
        SparseVoxelOctree& get_svo();

        // structure
        void rebuild_svo();
        void refresh_svo();
        void clear_svo(bool clear_setting);
        void generate_connector();
        void clear_connector();

        // bind test
        void set_info(float p_info);
        float get_info();

        // path finding
        Array path_result;
        Array get_last_path_result();
        // v1
        Array find_path_v1(const Vector3 start, const Vector3 end, float agent_r, bool is_smooth = true);
        // v2
        bool find_raw_path(Vector3 start, Vector3 end, float agent_r);
        bool find_raw_path_v2(Vector3 start, Vector3 end, float agent_r);
        void smooth_path_string_pulling_fast(float agent_radius);
        void smooth_path_string_pulling_fast_v2(float agent_radius);
        void smooth_path_string_pulling_full(float agent_radius);
        void smooth_path_string_pulling_full_v2(float agent_radius);
        void smooth_path_string_pulling_v3(float agent_radius);
        bool direct_path_check(const Vector3 start, const Vector3 end, float agent_r);
        void find_path_v2(const Vector3 start, const Vector3 end, float agent_r, bool is_smooth = true);
        void find_path_multi_thread(const Vector3 start, const Vector3 end, float agent_r, bool is_smooth = true);
        void transfer_path_result();
        // jump
        //bool find_raw_path_JP(Vector3 start, Vector3 end, float agent_r);

        // override
        void _init();
        void _ready();
        void _process(double delta);
        void _physics_process(double delta);
        void _enter_tree();
        void _exit_tree();
    };

}

#endif // SVO_NAVMAP_H