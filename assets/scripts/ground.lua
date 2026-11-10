local W, H = 48, 40
local GROUND_MARGIN = 3

register_monster("ground.lua", function(x, _y)
    return spawn({
        name = "GroundUnit",
        position = { x = x, y = SCREEN_H - H - GROUND_MARGIN },
        hitbox = { w = W, h = H },
        velocity = { x = -120, y = 0 },
        renderable = { r = 120, g = 200, b = 60, a = 255 },
        sprite = { texture = "assets/enemy.gif", col = 0, row = 0, w = FRAME_W, h = FRAME_H },
        animation  = { frames = 9, cols = 1, fps = 6 },
        enemy = {},
        enemy_shooter = { weapon = "ground_shot" },
    })
end)
