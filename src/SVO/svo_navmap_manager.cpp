#include "svo_navmap_manager.h"

using namespace godot;

void SvoNavmapManager::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("acquire_navmesh"), &SvoNavmapManager::acquire_navmesh);
    ClassDB::bind_method(D_METHOD("tidy_adjacents"), &SvoNavmapManager::tidy_adjacents);
}

SvoNavmapManager::SvoNavmapManager()
{

}
SvoNavmapManager::~SvoNavmapManager() {
    // 由于SvoNavmesh被加入场景树，用SvoNavmeshManager管理可能会造成混乱
}

void SvoNavmapManager::acquire_navmesh(SvoNavmap* navmesh) {
    navmeshes[navmesh->uuid] = navmesh;
}

bool SvoNavmapManager::query(Vector3 position) const {
    Array keys = navmeshes.keys();
    for (int i = 0; i < keys.size(); i++) {
        SvoNavmap* navmesh = Object::cast_to<SvoNavmap>(navmeshes[keys[i]]);
        if (navmesh && navmesh->query_voxel(position)) {
            return true;
        }
    }
    return false;
}

SvoNavmap* SvoNavmapManager::find_navmesh_containing(Vector3 position) const {
    Array keys = navmeshes.keys();
    for (int i = 0; i < keys.size(); i++) {
        SvoNavmap* navmesh = Object::cast_to<SvoNavmap>(navmeshes[keys[i]]);
        if (navmesh && navmesh->query_voxel(position)) {
            return navmesh;
        }
    }
    return nullptr;
}

Vector<Vector3> SvoNavmapManager::find_path(Vector3 start, Vector3 end) const {
    SvoNavmap* start_navmesh = find_navmesh_containing(start);
    SvoNavmap* end_navmesh = find_navmesh_containing(end);

    if (!start_navmesh || !end_navmesh) {
        return {}; // 如果起点或终点不在任何一个Navmesh中，则返回空路径
    }

    if (start_navmesh == end_navmesh) {
        // 如果起点和终点在同一个Navmesh中，直接在该Navmesh中寻路
        // 需要实现SparseVoxelOctree::findPath方法
        //return start_navmesh->get_svo().find_path(start, end);
    }

    // 否则，需要处理跨多个Navmesh的寻路
    // 简化处理：找到相邻Navmesh之间的连接点，进行A*或其他路径规划
    Vector<Vector3> path;

    // 这里只是一个示例，实际实现会更加复杂
    path.push_back(start);
    path.push_back(Vector3(0, 0, 0)); // Placeholder for boundary point
    path.push_back(end);

    return path;
}

void SvoNavmapManager::tidy_adjacents() {
    auto roots = find_possible_roots();
    HashSet<String> visited;

    for (int i = 0; i < roots.size(); i++) {
        String root = roots[i];
        if (!visited.has(root)) {  // 使用 .has() 方法检查元素是否存在
            bfs(root, visited);
        }
    }
}

void SvoNavmapManager::bfs(String start_uuid, HashSet<String>& visited) {
    std::queue<String> queue;
    queue.push(start_uuid);
    visited.insert(start_uuid);

    while (!queue.empty()) {
        String current_uuid = queue.front();
        queue.pop();

        // 安全地获取当前 navmesh
        SvoNavmap* current_navmesh = Object::cast_to<SvoNavmap>(navmeshes[current_uuid]);
        if (!current_navmesh) continue;  // 如果转换失败或navmesh不存在，则跳过

        Array adjacent_uuids = current_navmesh->adjacent_uuids;
        UtilityFunctions::print("Processing: " + current_uuid);
        Transform3D current_transform = current_navmesh->get_transform();
        float voxel_size = current_navmesh->get_voxel_size();

        for (int i = 0; i < adjacent_uuids.size(); i++) {
            Variant adj_uuid_var = adjacent_uuids[i];

            // 检查 UUID 是否为有效的字符串
            if (adj_uuid_var.get_type() == Variant::STRING) {
                String adj_uuid = adj_uuid_var;

                // 检查 UUID 字符串是否为空并且该 UUID 是否已被访问
                if (!adj_uuid.is_empty() && !visited.has(adj_uuid)) {
                    // 正确地获取相邻的 navmesh
                    SvoNavmap* adj_navmesh = Object::cast_to<SvoNavmap>(navmeshes[adj_uuid]);
                    if (!adj_navmesh) continue;  // 如果转换失败或navmesh不存在，则跳过

                    visited.insert(adj_uuid);
                    queue.push(adj_uuid);

                    // 计算位置偏移
                    Vector3 local_offset;
                    switch (i) {
                    case 0: local_offset = Vector3(voxel_size, 0, 0); break;
                    case 1: local_offset = Vector3(-voxel_size, 0, 0); break;
                    case 2: local_offset = Vector3(0, voxel_size, 0); break;
                    case 3: local_offset = Vector3(0, -voxel_size, 0); break;
                    case 4: local_offset = Vector3(0, 0, voxel_size); break;
                    case 5: local_offset = Vector3(0, 0, -voxel_size); break;
                    }

                    // 将局部偏移转换为全局偏移
                    Vector3 global_offset = current_transform.basis.xform(local_offset);
                    Transform3D new_transform = Transform3D(current_transform.basis, current_transform.origin + global_offset);
                    adj_navmesh->set_transform(new_transform);
                    adj_navmesh->set_voxel_size(current_navmesh->get_voxel_size());
                    adj_navmesh->refresh_svo();
                    UtilityFunctions::print(vformat("Adjust SVO: " + adj_uuid));
                }
            }
        }
    }
}

Array SvoNavmapManager::find_possible_roots() {
    // 将所有SvoNavmesh UUID添加到潜在根集合中
    Array keys = navmeshes.keys();
    Array potential_roots = keys.duplicate();

    // 从潜在根集合中移除任何被引用的UUID
    for (int i = 0; i < keys.size(); i++) {
        Array adjacent_uuids = Object::cast_to<SvoNavmap>(navmeshes[keys[i]])->adjacent_uuids;
        for (int j = 0; j < adjacent_uuids.size(); j++) {
            Variant adj_uuid_var = adjacent_uuids[j];
            // 检查adj_uuid_var是否为有效的UUID
            if (adj_uuid_var.get_type() == Variant::STRING) {
                String adj_uuid = adj_uuid_var;
                if (!adj_uuid.is_empty() && navmeshes.has(adj_uuid)) {
                    potential_roots.erase(adj_uuid);
                }
            }
        }
    }
    return potential_roots;
}