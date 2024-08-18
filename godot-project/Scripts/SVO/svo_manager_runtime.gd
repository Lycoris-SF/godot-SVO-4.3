@tool
extends SvoNavmapManager

func _init():
		pass

func _ready():
	if self.is_inside_tree():
		populate_navmeshes()
		print("All SvoNavmapes have been acquired by manager.")
	
func _input(event):
	# Receives key input
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_E:
			if self is SvoNavmapManager and self.is_inside_tree():
				self._pathfinding_done()
				for uuid in MinosUUIDGenerator.usedUUID:
					print("exist uuid: " + uuid)

func populate_navmeshes():
	var root = get_tree().get_root()
	collect_navmeshes(root)

func collect_navmeshes(node):
	for child in node.get_children():
		if child is SvoNavmap:
			self.acquire_navmesh(child)
		collect_navmeshes(child)

func _pathfinding_done():
	var path #= self.get_last_path_result()
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
