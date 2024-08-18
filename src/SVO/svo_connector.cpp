#include "svo_connector.h"

using namespace godot;

void SvoConnector::_bind_methods()
{
}

SvoConnector::SvoConnector() : debugMesh(nullptr)
{
}
SvoConnector::~SvoConnector()
{
    if (debugMesh) { 
        if(debugMesh->is_inside_tree()) remove_child(debugMesh);
        debugMesh->queue_free(); 
    }
}

static void init_static_material() {
    if (debugConnectorMaterialR.is_null()) debugConnectorMaterialR.instantiate();
    debugConnectorMaterialR->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
    debugConnectorMaterialR->set_albedo(Color(0.7, 0.1, 0.1, 0.5));  // °ëÍ¸Ã÷ºìÉ«
}
static void clear_static_material() {
    // static Mat is not freed after level switch
    // TODO: change this when ready for game
    debugConnectorMaterialR.unref();
}
void SvoConnector::_enter_tree()
{
    init_static_material();
}

void SvoConnector::_exit_tree()
{
    clear_static_material();
}

void SvoConnector::init_debugMesh(float agent_r)
{
    Ref<SphereMesh> sphereMesh = memnew(SphereMesh);
    sphereMesh->set_radius(MAX(agent_r, 0.02f));
    sphereMesh->set_height(MAX(agent_r, 0.02f) * 2.0f);

    MeshInstance3D* debugMesh = memnew(MeshInstance3D);
    debugMesh->set_mesh(sphereMesh);
    debugMesh->set_material_override(debugConnectorMaterialR);
    add_child(debugMesh);
}