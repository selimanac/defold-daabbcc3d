local camera         = {}

local DISPLAY_WIDTH  = sys.get_config_int("display.width")
local DISPLAY_HEIGHT = sys.get_config_int("display.height")
local camera_url     = msg.url(".")
local VECTOR_UP      = vmath.vector3(0, 1, 0)
local PIVOT          = vmath.vector3(0, 1, 0)

local nv             = vmath.vector4(0, 0, -1, 1)
local fv             = vmath.vector4(0, 0, 1, 1)

function camera.init()
	msg.post("@render:", "use_camera_projection")

	camera_url = msg.url(".")
	camera_url.path = "/container/camera"
	camera_url.fragment = "camera"
end

--[[function camera.screen_to_world(x, y)

	if delta then
		x = x / M.viewport.scale.x
		y = y / M.viewport.scale.y
	else
		x = (x - M.viewport.x) / M.viewport.scale.x
		y = (y - M.viewport.y) / M.viewport.scale.y
	end

	local screenPosition = vmath.vector3(x, y, z)

	local world_position = vmath.vector3()
	local projection = go.get(camera_url, "projection")
	local view = go.get(camera_url, "view")
	local w, h = window.get_size()

	local m = vmath.inv(projection * view)

	local x1 = (screenPosition.x - w * 0.5) / w * 2
	local y1 = (screenPosition.y - h * 0.5) / h * 2

	nv.x, nv.y = x1, y1
	fv.x, fv.y = x1, y1
	local np = m * nv
	local fp = m * fv
	np = np * (1 / np.w)
	fp = fp * (1 / fp.w)

	local npp = vmath.vector3(np.x, np.y, np.z)
	local fpp = vmath.vector3(fp.x, fp.y, fp.z)

	local denom = vmath.dot(VECTOR_UP, fpp - npp)

	if denom == 0 then
		return
	else
		local numer = vmath.dot(VECTOR_UP, PIVOT - npp)
		return vmath.lerp(numer / denom, npp, fpp)
	end
end]]

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

function camera.screen_to_world_2d(x, y, delta, worldz, raw)
	--	x, y = camera.screen_to_viewport(x, y, delta)


	local projection = go.get(camera_url, "projection")
	local view = go.get(camera_url, "view")
	local window_width, window_height = window.get_size()

	local m = not delta and vmath.inv(projection * view) or vmath.inv(projection)

	-- Remap coordinates to range -1 to 1
	x1 = (x - window_width * 0.5) / window_width * 2
	y1 = (y - window_height * 0.5) / window_height * 2

	if delta then
		x1 = x1 + 1; y1 = y1 + 1
	end

	nv.x, nv.y = x1, y1
	fv.x, fv.y = x1, y1
	local np = m * nv
	local fp = m * fv
	np = np * (1 / np.w)
	fp = fp * (1 / fp.w)

	local t = (worldz - 0.1) / (100 - 0.1) -- normalize desired Z to 0-1 from abs_nearZ to abs_farZ
	local worldpos = vmath.lerp(t, np, fp)

	if raw then
		return worldpos.x, worldpos.y, worldpos.z
	else
		return vmath.vector3(worldpos.x, worldpos.y, worldpos.z)
	end -- convert vector4 to vector3
end

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
