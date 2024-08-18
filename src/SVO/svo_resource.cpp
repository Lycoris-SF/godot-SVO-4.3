#include "svo_resource.h"

using namespace godot;

SparseVoxelOctreeResource::SparseVoxelOctreeResource()
{

}

void godot::SparseVoxelOctreeResource::save()
{
	//Ref<SparseVoxelOctreeResource> svo_resource = memnew(SparseVoxelOctreeResource);
	//svo_resource->svo = my_svo_instance;  // 将SVO实例赋值给资源
	//ResourceSaver::save("res://path_to_save/svo_resource.tres", svo_resource);
}

void godot::SparseVoxelOctreeResource::read()
{
	//Ref<SparseVoxelOctreeResource> loaded_resource = ResourceLoader::load("res://path_to_save/svo_resource.tres");
	//SparseVoxelOctree loaded_svo = loaded_resource->svo;
}
