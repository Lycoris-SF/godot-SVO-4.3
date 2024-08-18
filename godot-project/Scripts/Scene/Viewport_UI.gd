extends Control

func _ready():
	$SpinBox_MinDepth.value = $"../../../../spider exp project/SvoNavmap".DrawRef_minDepth
	$SpinBox_MaxDepth.value = $"../../../../spider exp project/SvoNavmap".DrawRef_maxDepth
	$".".visible = GlobalValue.svo_debugMode

func _on_smooth_path_changed(is_active: bool):
	$"../../../../NavAgent".is_smooth = is_active

func _on_show_empty_changed(is_active: bool):
	$"../../../../spider exp project/SvoNavmap".show_empty = is_active

func _on_show_path_changed(is_active: bool):
	$"../../../../spider exp project/SvoNavmap".show_path = is_active

func _on_min_depth_changed(depth: float):
	$"../../../../spider exp project/SvoNavmap".DrawRef_minDepth = int(depth)
	$SpinBox_MinDepth.value = $"../../../../spider exp project/SvoNavmap".DrawRef_minDepth
	
func _on_max_depth_changed(depth: float):
	$"../../../../spider exp project/SvoNavmap".DrawRef_maxDepth = int(depth)
	$SpinBox_MaxDepth.value = $"../../../../spider exp project/SvoNavmap".DrawRef_maxDepth
