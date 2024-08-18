#ifndef SVO_CONNECTOR_H
#define SVO_CONNECTOR_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node3d.hpp>

#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/vector3.hpp>

// debugMesh
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/sphere_mesh.hpp>

namespace godot {
	class SvoConnector;
	static Ref<StandardMaterial3D> debugConnectorMaterialR;

	struct ConnectorPath {
		SvoConnector* target;
		Vector<Vector3> path;

		ConnectorPath(SvoConnector* c, Vector<Vector3> p) : target(c), path(p) {}
	};

	class SvoConnector : public Node3D {
		GDCLASS(SvoConnector, Node3D)
	private:
		Vector<ConnectorPath> neighbors;
		MeshInstance3D* debugMesh;

	public:
		SvoConnector();
		~SvoConnector();

		void init_debugMesh(float agent_r);

		void _enter_tree();
		void _exit_tree();

	protected:
		static void _bind_methods();
	};
}

#endif // SVO_CONNECTOR_H