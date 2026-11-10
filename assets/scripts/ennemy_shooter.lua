local FRAME_W = 190
local FRAME_H = 213
local SCALE = 0.35

register_monster("ennemy_shooter.lua", function(x, y)
    return spawn({
        name = "Enemy",
        position = { x = x, y = y },
        hitbox = { w = FRAME_W * SCALE, h = FRAME_H * SCALE },
        velocity = { x = -200, y = 0 },
        renderable = { r = 255, g = 0, b = 0, a = 255 },
        sprite = { texture = "assets/enemy.gif", col = 0, row = 0, w = FRAME_W, h = FRAME_H },
        animation  = { frames = 9, cols = 1, fps = 6 },
        enemy = {},
        enemy_shooter = { weapon = "enemy_basic" },
    })
end)
