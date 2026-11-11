local SPAWN_INTERVAL = 5
local timer = 0
local FRAME_W, FRAME_H = 33, 33
local SCALE = 2.5

local function spawn_enemy(x, y)
    return spawn({
        name          = "Ground Enemy",
        position      = { x = x, y = y },
        stat          = { life = 100 },
        hitbox        = { w = FRAME_W * SCALE, h = FRAME_H * SCALE },
        velocity      = { x = -200, y = 0 },
        renderable    = { r = 255, g = 0, b = 0, a = 255 },
        sprite        = { texture = "assets/groundMob.gif", col = 0, row = 0, w = FRAME_W, h = FRAME_H },
        animation     = { frames = 3, cols = 3, fps = 1 },
        enemy         = {},
        enemy_shooter = { weapon = "enemy_basic" },
    })
end

on_update(function(dt)
    timer = timer + dt
    if timer >= SPAWN_INTERVAL then
        timer = timer - SPAWN_INTERVAL
        spawn_enemy(SCREEN_W, SCREEN_H - (FRAME_H * SCALE))
    end
end)
