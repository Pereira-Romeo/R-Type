WEAPONS = WEAPONS or {}

local function bullet(x, y, vx, vy, src, w, h)
    return spawn({
        name       = "Bullet",
        position   = { x = x, y = y },
        hitbox     = { w = w, h = h },
        velocity   = { x = vx, y = vy },
        renderable = { r = 255, g = 0, b = 0, a = 255 },
        sprite     = { texture = "assets/shoot.gif",
                       src = { x = src.x, y = src.y, w = src.w, h = src.h } },
        projectile = {},
    })
end

local BASIC_SRC = { x = 237, y = 13, w = 26, h = 8 }
WEAPONS.basic = {
    interval = 0.2,
    fire = function(x, y)
        local w, h = BASIC_SRC.w *2, BASIC_SRC.h * 2
        bullet(x, y - h / 2, 500, 0, BASIC_SRC, w, h)
    end,
}

local SPREAD_SRC = { x = 184, y = 9, w = 16, h = 16 }
WEAPONS.spread = {
    interval = 0.8,
    fire = function(x, y)
        local w, h = SPREAD_SRC.w , SPREAD_SRC.h
        for _, vy in ipairs({ -150, 0, 150 }) do
            bullet(x, y - h / 2, 500, vy, SPREAD_SRC, w, h)
        end

    end,
}

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
