local collision          = {}

collision.collision_bits = {
	PLAYER  = 1, -- (2^0)
	ENEMY   = 2, -- (2^1)
	GROUND  = 4, -- (2^2)
	ITEM    = 8, -- (2^3)
	WALL    = 16, -- (2^4)
	POINTER = 32,
	BUTTON  = 64,
	ALL     = bit.bnot(0) -- -1 for all results
}

local default_mask_bits  = bit.bor(collision.collision_bits.PLAYER, collision.collision_bits.ENEMY, collision.collision_bits.GROUND, collision.collision_bits.ITEM, collision.collision_bits.WALL)
local selected_mask_bits = bit.bor(collision.collision_bits.ENEMY, collision.collision_bits.ITEM)
local btn_mask_bits      = bit.bor(collision.collision_bits.BUTTON)
local aabb_group_id      = 0

function collision.init()
	aabb_group_id = daabbcc3d.new_group(daabbcc3d.UPDATE_PARTIALREBUILD)
end

function collision.insert_aabb(position, width, height, depth, collision_bit)
	collision_bit = collision_bit and collision_bit or nil
	return daabbcc3d.insert_aabb(aabb_group_id, position, width, height, depth, collision_bit)
end

function collision.insert_gameobject(go_url, width, height, depth, collision_bit)
	collision_bit = collision_bit and collision_bit or nil
	return daabbcc3d.insert_gameobject(aabb_group_id, go_url, width, height, depth, collision_bit)
end

function collision.query_aabb(position, width, height, depth, is_mask, get_manifold)
	local mask_bits = is_mask and selected_mask_bits or default_mask_bits
	get_manifold    = get_manifold and get_manifold or nil
	return daabbcc3d.query_aabb(aabb_group_id, position, width, height, depth, mask_bits, get_manifold, is_mask)
end

function collision.query_id(aabb_id, is_mask, get_manifold)
	local mask_bits = is_mask and selected_mask_bits or default_mask_bits
	get_manifold    = get_manifold and get_manifold or nil
	return daabbcc3d.query_id(aabb_group_id, aabb_id, mask_bits, get_manifold, is_mask)
end

function collision.query_id_btn(aabb_id)
	return daabbcc3d.query_id(aabb_group_id, aabb_id, btn_mask_bits)
end

function collision.query_id_sort(aabb_id, is_mask, get_manifold)
	local mask_bits = is_mask and selected_mask_bits or default_mask_bits
	get_manifold    = get_manifold and get_manifold or nil
	return daabbcc3d.query_id_sort(aabb_group_id, aabb_id, mask_bits, get_manifold, is_mask)
end

function collision.query_aabb_sort(position, width, height, depth, is_mask, get_manifold)
	local mask_bits = is_mask and selected_mask_bits or default_mask_bits
	get_manifold    = get_manifold and get_manifold or nil
	return daabbcc3d.query_aabb_sort(aabb_group_id, position, width, height, depth, mask_bits, get_manifold, is_mask)
end

function collision.raycast(ray_start, ray_end, is_mask, get_manifold)
	local mask_bits = is_mask and selected_mask_bits or default_mask_bits
	get_manifold    = get_manifold and get_manifold or nil
	return daabbcc3d.raycast(aabb_group_id, ray_start, ray_end, mask_bits, get_manifold, is_mask)
end

function collision.raycast_sort(ray_start, ray_end, is_mask, get_manifold)
	local mask_bits = is_mask and selected_mask_bits or default_mask_bits
	get_manifold    = get_manifold and get_manifold or nil
	return daabbcc3d.raycast_sort(aabb_group_id, ray_start, ray_end, mask_bits, get_manifold, is_mask)
end

function collision.update_aabb(aabb)
	daabbcc3d.update_aabb(aabb_group_id, aabb.aabb_id, aabb.position, aabb.size.width, aabb.size.height, aabb.size.depth)
end

function collision.reset()
	daabbcc3d.reset()
end

return collision
