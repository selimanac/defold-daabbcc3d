local collision                  = require("example.scripts.lib.collision")
local camera                     = require("example.scripts.lib.camera")

local manager                    = {}
local info_label_url             = msg.url(".")
local collection_title_label_url = msg.url(".")
local planeNormal                = vmath.vector3(0, 0, 1)
local planePoint                 = vmath.vector3(0, 0, 0)
local info_string                = "0 - Reset / 1 - Sort: %s / 2 - Manifold: %s / 3 - Mask: %s"
local KEYS                       = {
	AABB_DEFAULT = hash("aabb_default"),
	AABB_SORT = hash("aabb_sort"),
	AABB_MANIFOLD = hash("aabb_manifold"),
	AABB_MASK = hash("aabb_mask"),
}

manager.world_position           = vmath.vector3()
manager.is_sort                  = false
manager.is_manifold              = false
manager.is_mask                  = false

local function set_info_text()
	label.set_text(info_label_url, string.format(info_string, tostring(manager.is_sort), tostring(manager.is_manifold), tostring(manager.is_mask)))
end

local function toogle_sort()
	manager.is_sort = not manager.is_sort and true or false
	set_info_text()
end

local function toogle_manifold()
	manager.is_manifold = not manager.is_manifold and true or false
	set_info_text()
end

function manager.toogle_manifold()
	toogle_manifold()
end

local function toogle_mask()
	manager.is_mask = not manager.is_mask and true or false
	set_info_text()
end

local function toogle_default()
	manager.is_sort     = false
	manager.is_manifold = false
	manager.is_mask     = false
	set_info_text()
end

function manager.init(title_txt)
	msg.post(".", "acquire_input_focus")
	msg.post("@render:", "clear_color", { color = vmath.vector4(100 / 255, 100 / 255, 100 / 255, 0) })

	info_label_url                      = msg.url(".")
	info_label_url.path                 = "/container/info"
	info_label_url.fragment             = "label"

	collection_title_label_url          = msg.url(".")
	collection_title_label_url.path     = "/container/info"
	collection_title_label_url.fragment = "collection_txt"
	label.set_text(collection_title_label_url, title_txt)

	camera.init()
	collision.init()

	set_info_text()
end

function manager.input(action_id, action)
	manager.world_position = camera.screen_to_world_plane(action.x, action.y, planeNormal, planePoint)

	if action_id == KEYS.AABB_DEFAULT and action.pressed then
		toogle_default()
	elseif action_id == KEYS.AABB_SORT and action.pressed then
		toogle_sort()
	elseif action_id == KEYS.AABB_MANIFOLD and action.pressed then
		toogle_manifold()
	elseif action_id == KEYS.AABB_MASK and action.pressed then
		toogle_mask()
	end
end

function manager.final()
	manager.world_position = vmath.vector3()
	manager.is_sort        = false
	manager.is_manifold    = false
	manager.is_mask        = false

	collision.reset()
end

return manager
