extends Node3D
	
func _input(event):
	# Receives key input
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_ESCAPE:
			get_tree().change_scene_to_file("res://Scene/Start_Scene.tscn")
