<<<<<<< HEAD:assets/scripts/02_ennemy.lua
local FRAME_W = 190
local FRAME_H = 213
local SCALE = 0.35
=======
local SPAWN_INTERVAL = 2
local timer = 0
local FRAME_W, FRAME_H = 33, 33
local SCALE = 2.5
>>>>>>> 6cafa62609628fc4d9771e9cd044f14a3b272b98:assets/scripts/enemy/02_flyingmob.lua

register_monster("02_ennemy.lua", function(x, y)
    return spawn({
<<<<<<< HEAD:assets/scripts/02_ennemy.lua
        name = "Enemy",
        position = { x = x, y = y },
        hitbox = { w = FRAME_W * SCALE, h = FRAME_H * SCALE },
        velocity = { x = -200, y = 0 },
        renderable = { r = 255, g = 0, b = 0, a = 255 },
        sprite = { texture = "assets/enemy.gif", col = 0, row = 0, w = FRAME_W, h = FRAME_H },
        animation = { frames = 9, cols = 1, fps = 6 },
        enemy = {},
=======
        name          = "Flying Enemy",
        position      = { x = x, y = y },
        stat          = { life = 100 },
        hitbox        = { w = FRAME_W * SCALE, h = FRAME_H * SCALE },
        velocity      = { x = -200, y = 0 },
        renderable    = { r = 255, g = 0, b = 0, a = 255 },
        sprite        = { texture = "assets/flyingmob.gif", col = 0, row = 0, w = FRAME_W, h = FRAME_H },
        animation     = { frames = 3, cols = 3, fps = 4 },
        enemy         = {},
        enemy_shooter = { weapon = "enemy_basic" },
>>>>>>> 6cafa62609628fc4d9771e9cd044f14a3b272b98:assets/scripts/enemy/02_flyingmob.lua
    })
end)
