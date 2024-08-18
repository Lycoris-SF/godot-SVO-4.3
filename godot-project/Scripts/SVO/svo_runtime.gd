@tool
extends SvoNavmap

func _init():
	if self.uuid == "":
		self.uuid = MinosUUIDGenerator.generate_new_UUID()
		print("New UUID in svo: ", self.uuid)

func _ready():
	if self.is_visible_in_tree():
		self.manual_set_node_ready()
		self.clear_svo(false)
		self.refresh_svo()
		if not Engine.is_editor_hint():
			self.debug_mode = GlobalValue.svo_debugMode
			self.rebuild_svo()
	
func _input(event):
	# Receives key input
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_E:
			pass

func _pathfinding_done():
	var path = self.get_last_path_result()
	if path.is_empty():
		print("No Path exist!")
	else:
		print("Existing Path: ")
		_print_formatted_path(path)
		
func _print_formatted_path(path):
	var formatted_path = []
	for point in path:
		formatted_path.append("(%f, %f, %f)" % [point.x, point.y, point.z])
	print("[%s]" % ", ".join(formatted_path))
