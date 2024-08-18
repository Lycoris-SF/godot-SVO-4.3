extends RigidBody3D

signal A_star_completed
signal Path_found

# thread(only one in background)
var thread: Thread
var is_smooth: bool
var is_physics_task_set = false

# Microbenchmarking 
var begin_time_u
var begin_time_m
var end_time_u
var end_time_m

# steering
var agent_r_temp = 0.125
var svo : SvoNavmap
var path = []
var current_target_index = 0
var close_enough_radius = 0.125/2
var max_speed = 1.25
var arrival_threshold = 0.125*5		# Distance threshold for deceleration to start
var max_acceleration = 9.81 * 1.0	# Maximum acceleration, 3G
var max_deceleration = 9.81 * 0.5	# Maximum deceleration, 1G
var max_torque: float
var max_angular_speed = 36.0		# Degree/s
var angular_acceleration = 12.0		# Degree/s^2

var debugLabel1
var debugLabel2
var debugLabel3
var debugLabel4
var debugLabel5

func _ready():
	is_smooth = true
	thread = Thread.new()
	# temp solution, replace with mamager when ready for games
	svo = $"../spider exp project/SvoNavmap"
	A_star_completed.connect(_on_A_star_completed)
	Path_found.connect(_on_path_found)
	
	debugLabel1 = $"../SubViewportContainer/SubViewport/UI/VBoxContainerR/Label"
	debugLabel2 = $"../SubViewportContainer/SubViewport/UI/VBoxContainerR/Label2"
	debugLabel3 = $"../SubViewportContainer/SubViewport/UI/VBoxContainerR/Label3"
	debugLabel4 = $"../SubViewportContainer/SubViewport/UI/VBoxContainerR/Label4"
	debugLabel5 = $"../SubViewportContainer/SubViewport/UI/VBoxContainerR/Label5"
	
	max_torque = 9.81 * 3 * 0.3 * self.mass
	max_angular_speed = deg_to_rad(max_angular_speed)
	angular_acceleration = deg_to_rad(angular_acceleration)
		
func _physics_process(delta):
	if is_physics_task_set:
		PF_physics_task()
			
func _exit_tree():
	thread.wait_to_finish()

func clamp_vector3(v: Vector3, max_length: float) -> Vector3:
	if v.length() > max_length:
		return v.normalized() * max_length
	return v

func _integrate_forces(state):
	if path.size() > 0 and current_target_index < path.size():
		var target = path[current_target_index]
		var to_target = target - self.position
		var distance = to_target.length()

		if distance < close_enough_radius:
			current_target_index += 1
			if current_target_index >= path.size():
				print("Path completed!")
				return
		
		to_target = to_target.normalized()
	# rotate v1
		#var forward = global_transform.basis.z.normalized()
		#var cross_prod = forward.cross(to_target)
		#var dot_prod = forward.dot(to_target)
		#var q = Quaternion(cross_prod.x, cross_prod.y, cross_prod.z, 1.0 + dot_prod).normalized()
		#var angle = 2 * acos(q.w)
		#var axis = q.get_axis().normalized()
		#if angle > 0.01:
			#var torque = axis * angle * angle * state.step / 1500
			#state.apply_torque_impulse(torque)
			
	# rotate v2
		var forward_vector = global_transform.basis.z.normalized()
		var rotation_axis = forward_vector.cross(to_target)
		var dot_product = forward_vector.dot(to_target)
		var angle_diff = acos(clamp(dot_product, -1.0, 1.0))
		var angle_diff_degrees = rad_to_deg(angle_diff)

		var angular_velocity_magnitude = self.angular_velocity.length()
		var desired_angular_velocity_change

		# a steering working on bugs
		if angle_diff_degrees > 90.0:
			desired_angular_velocity_change = rotation_axis.normalized() * angular_acceleration * state.step
		elif angle_diff_degrees > 45.0:
			var alignment_scale = angle_diff_degrees / 90.0
			desired_angular_velocity_change = rotation_axis.normalized() * angular_acceleration * state.step
			desired_angular_velocity_change *= alignment_scale
		elif angle_diff_degrees > 0.15:
			#debugLabel1.text = "angular_velocity_magnitude: " + str(angular_velocity_magnitude)
			#debugLabel2.text = "angular_velocity_threshold: " + str(max_angular_speed * (angle_diff_degrees / 4.6))
			if angular_velocity_magnitude > max_angular_speed * (angle_diff_degrees / 3):
				#debugLabel4.text = "condition3"
				var alignment_scale = angle_diff_degrees / 3
				desired_angular_velocity_change = -self.angular_velocity.normalized() * angular_acceleration * alignment_scale * state.step
				#print(desired_angular_velocity_change)
			else:
				#debugLabel4.text = "condition4"
				#print(angle_diff_degrees / 20)
				var alignment_scale = max(0.1, angle_diff_degrees/20)
				desired_angular_velocity_change = rotation_axis.normalized() * angular_acceleration * alignment_scale * state.step
		else:
			#debugLabel4.text = "condition5"
			desired_angular_velocity_change = Vector3(0,0,0)
		#debugLabel3.text = "length: " + str(desired_angular_velocity_change.length())

		var new_angular_velocity = self.angular_velocity + desired_angular_velocity_change

		var torque_impulse = (new_angular_velocity - self.angular_velocity)
		torque_impulse = clamp_vector3(torque_impulse, max_torque * state.step)
		state.apply_torque_impulse(torque_impulse)
		
	# movement
		var target_speed = max_speed
		if distance < arrival_threshold:
			# Linear deceleration to 0
			target_speed = lerp(max_speed, max_speed/5, 1.0 - distance / arrival_threshold)

		var desired_velocity = to_target * target_speed
		var velocity_change = desired_velocity - state.linear_velocity
		var impulse = velocity_change * mass / state.step	# Normalize impulses using the physics step size

		# Different limits apply depending on whether you are accelerating or decelerating
		var current_speed = state.linear_velocity.length()
		var impulse_magnitude = impulse.length()
		if desired_velocity.length() > current_speed:
			# accelerating
			var max_impulse = max_acceleration * mass * state.step
			if impulse_magnitude > max_impulse:
				impulse = impulse.normalized() * max_impulse
		else:
			# decelerating
			var max_impulse = max_deceleration * mass * state.step
			if impulse_magnitude > max_impulse:
				impulse = impulse.normalized() * max_impulse

		# apply impulses
		state.apply_central_impulse(impulse)

func _input(event):
	# Receives key input
	if event is InputEventKey and event.pressed:
		# it's too early for multi thread
		if event.keycode == KEY_F:
			var rand_empty_in_svo = _find_rand_empty_pos()
			current_target_index = 0
			svo.find_path(self.position, rand_empty_in_svo, agent_r_temp, is_smooth)
			path = svo.get_last_path_result()
		if event.keycode == KEY_M:
			find_path_multi_thread()

func record_time(string):
	if end_time_m - begin_time_m < 1:
		print(string, end_time_u - begin_time_u, " microseconds")
	else:
		print(string, end_time_m - begin_time_m, " milliseconds")

func _find_rand_empty_pos():
	var root_size = svo.rootVoxelSize
	var root_pos = svo.global_position
	var half_size = root_size * 0.5
	var random_point = Vector3()
	var max_attempts = 100
	var attempts = 0
	
	while attempts < max_attempts:
		var random_x = randf_range(-half_size, half_size)
		var random_y = randf_range(-half_size, half_size)
		var random_z = randf_range(-half_size, half_size)
		random_point = Vector3(random_x, random_y, random_z) + root_pos
		
		if not svo.query_voxel(random_point):
			return random_point
			
		attempts += 1
		
	print("Failed to find an empty position after", max_attempts, "attempts.")
	return null

func find_path_multi_thread():
	current_target_index = 0
	path.clear()
	var rand_empty_in_svo = _find_rand_empty_pos()
	var data = [self.position, rand_empty_in_svo, agent_r_temp]
	var direct_path = svo.direct_path_check(self.position, rand_empty_in_svo, agent_r_temp)
	if not direct_path:
		thread.start(A_star.bind(data))
	else:
		emit_signal("Path_found")
		
func A_star(data):
	begin_time_u = Time.get_ticks_usec()
	begin_time_m = Time.get_ticks_msec()

	if not svo.find_raw_path(data[0],data[1],data[2]):
		print("Path finding failed!")
		call_deferred("emit_signal", "A_star_completed")
		return false
	else:
		call_deferred("emit_signal", "A_star_completed")
		end_time_u = Time.get_ticks_usec()
		end_time_m = Time.get_ticks_msec()
		record_time("Path finding took: ")
		return true

func _on_A_star_completed():
	var result = thread.wait_to_finish()
	is_physics_task_set = result and is_smooth
	if result and not is_physics_task_set:
		svo.transfer_path_result()
		init_debug_path()

func PF_physics_task():
	begin_time_u = Time.get_ticks_usec()
	begin_time_m = Time.get_ticks_msec()
	
	svo.smooth_path(agent_r_temp)
	svo.transfer_path_result()
	
	end_time_u = Time.get_ticks_usec()
	end_time_m = Time.get_ticks_msec()
	record_time("Smooth path took: ")
	
	is_physics_task_set = false
	call_deferred("init_debug_path")
	
func init_debug_path():
	if svo.debug_mode:
		begin_time_u = Time.get_ticks_usec()
		begin_time_m = Time.get_ticks_msec()
		
		svo.init_debug_path(agent_r_temp*svo.debug_path_scale)
		
		end_time_u = Time.get_ticks_usec()
		end_time_m = Time.get_ticks_msec()
		record_time("Init rendering took: ")
		
	emit_signal("Path_found")
		
func _on_path_found():
	path = svo.get_last_path_result()
