local camera     = {}

local camera_url = msg.url(".")
local VECTOR_UP  = vmath.vector3(0, 1, 0)
local PIVOT      = vmath.vector3(0, 1, 0)

local nv         = vmath.vector4(0, 0, -1, 1)
local fv         = vmath.vector4(0, 0, 1, 1)

function camera.init()
	msg.post("@render:", "use_camera_projection")

	camera_url = msg.url(".")
	camera_url.path = "/container/camera"
	camera_url.fragment = "camera"
end

function camera.screen_to_viewport(x, y, delta)
	if delta then
		x = x / M.viewport.scale.x
		y = y / M.viewport.scale.y
	else
		x = (x - M.viewport.x) / M.viewport.scale.x
		y = (y - M.viewport.y) / M.viewport.scale.y
	end
	return x, y
end

-- from rendercam
-- https://github.com/rgrams/rendercam
function camera.screen_to_world_ray(x, y, raw)
	local projection = go.get(camera_url, "projection")

	local view = go.get(camera_url, "view")

	local window_width, window_height = window.get_size()

	local m = vmath.inv(projection * view)

	-- Remap coordinates to range -1 to 1
	local x1 = (x - window_width * 0.5) / window_width * 2
	local y1 = (y - window_height * 0.5) / window_height * 2

	nv.x, nv.y = x1, y1
	fv.x, fv.y = x1, y1
	local np = m * nv
	local fp = m * fv
	np = np * (1 / np.w)
	fp = fp * (1 / fp.w)

	if raw then
		return np.x, np.y, np.z, fp.x, fp.y, fp.z
	else
		return vmath.vector3(np.x, np.y, np.z), vmath.vector3(fp.x, fp.y, fp.z)
	end
end

-- Gets screen to world ray and intersects it with a plane
function camera.screen_to_world_plane(x, y, planeNormal, pointOnPlane)
	local np, fp = camera.screen_to_world_ray(x, y)
	local denom = vmath.dot(planeNormal, fp - np)
	if denom == 0 then
		-- ray is perpendicular to plane normal, so there are either 0 or infinite intersections
		return
	end
	local numer = vmath.dot(planeNormal, pointOnPlane - np)
	return vmath.lerp(numer / denom, np, fp)
end

return camera
