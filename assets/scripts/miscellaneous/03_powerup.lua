local SPAWN_INTERVAL = 3.5
local timer = 0
local isspawned = 0
local FRAME_W, FRAME_H = 50, 50
local SCALE = 1

local function spawn_weaponup(x, y)
    return spawn({
        name          = "powerup",
        position      = { x = x, y = y },
        hitbox        = { w = FRAME_W * SCALE, h = FRAME_H * SCALE },
        renderable    = { r = 255, g = 0, b = 0, a = 255 },
        --sprite        = { texture = "assets/enemy.gif", col = 0, row = 0, w = FRAME_W, h = FRAME_H },
        powerup       = {},
    })
end

on_update(function(dt)
    timer = timer + dt
    if timer >= SPAWN_INTERVAL  and isspawned == 0 then
        isspawned = 1
        spawn_weaponup(SCREEN_W / 2 , SCREEN_H / 2)
    end
end)
