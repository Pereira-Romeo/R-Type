local SPAWN_INTERVAL = 1.5
local timer = 0
local FRAME_W, FRAME_H = 190, 213
local SCALE = 0.35

local function spawn_enemy(x, y)
    return spawn({
        name          = "Enemy",
        position      = { x = x, y = y },
        hitbox        = { w = FRAME_W * SCALE, h = FRAME_H * SCALE },
        velocity      = { x = -200, y = 0 },
        renderable    = { r = 255, g = 0, b = 0, a = 255 },
        sprite        = { texture = "assets/enemy.gif", col = 0, row = 0, w = FRAME_W, h = FRAME_H },
        animation     = { frames = 9, cols = 1, fps = 6 },
        enemy         = {},
        enemy_shooter = { weapon = "enemy_basic" },
    })
end

on_update(function(dt)
    timer = timer + dt
    if timer >= SPAWN_INTERVAL then
        timer = timer - SPAWN_INTERVAL
        spawn_enemy(SCREEN_W, math.random(0, math.floor(SCREEN_H - FRAME_H * SCALE)))
    end
end)
