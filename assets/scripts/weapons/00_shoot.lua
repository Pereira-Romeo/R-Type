WEAPONS = WEAPONS or {}

local function bullet(x, y, vx, vy, w, h, sprite)
    local def = {
        name       = "Bullet",
        position   = { x = x, y = y },
        hitbox     = { w = w, h = h },
        velocity   = { x = vx, y = vy },
        renderable = { r = 255, g = 0, b = 0, a = 255 },
        projectile = {},
    }
    if sprite.src then
        def.sprite = { texture = sprite.tex, src = sprite.src }
    else
        def.sprite    = { texture = sprite.tex, col = 0, row = 0,
                          w = sprite.w, h = sprite.h,
                          offset_x = sprite.ox, offset_y = sprite.oy }
        def.animation = sprite.anim
    end
    return spawn(def)
end

local BASIC = {
    tex = "assets/playerBullet/base.gif",
    src = { x = 39, y = 9, w = 16, h = 4 },
}
WEAPONS.basic = {
    interval = 0.2,
    fire = function(x, y)
        local w, h = BASIC.src.w * 3, BASIC.src.h * 3
        bullet(x, y - h / 2, 500, 0, w, h, BASIC)
    end,
}

local SPREAD = {
    tex = "assets/playerBullet/spread.gif",
    w = 17, h = 12, ox = 12, oy = 4,
    anim = { frames = 2, cols = 2, fps = 10 },
}
WEAPONS.spread = {
    interval = 0.2,
    fire = function(x, y)
        local w, h = SPREAD.w * 2, SPREAD.h * 2
        for _, vy in ipairs({ -150, 0, 150 }) do
            bullet(x, y - h / 2, 500, vy, w, h, SPREAD)
        end
    end,
}
