WEAPONS = WEAPONS or {}

-- ===== Enmy Shooter function =====
local function enemy_bullet(x, y, vx, vy, src, w, h)
    return spawn({
        name             = "EnemyBullet",
        position         = { x = x, y = y },
        hitbox           = { w = w, h = h },
        velocity         = { x = vx, y = vy },
        renderable       = { r = 255, g = 0, b = 0, a = 255 },
        sprite           = { texture = "assets/enemy_projectile.gif",
                             src = { x = src.x, y = src.y, w = src.w, h = src.h } },
        enemy_projectile = {},
    })
end

local ENEMY_SRC = { x = 32, y = 0, w = 17, h = 17 }
WEAPONS.enemy_basic = {
    interval = 0,
    fire = function(x, y)
        local w, h = ENEMY_SRC.w * 2, ENEMY_SRC.h * 2
        enemy_bullet(x - w, y - h / 2, -400, 0, ENEMY_SRC, w, h)
    end,
}
