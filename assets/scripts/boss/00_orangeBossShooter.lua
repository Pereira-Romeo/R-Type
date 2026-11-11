WEAPONS = WEAPONS or {}

-- ===== Orange Boss weapons =====
local function boss_bullet(tex, x, y, vx, vy, w, h, sprite, anim)
    return spawn({
        name             = "BossBullet",
        position         = { x = x, y = y },
        hitbox           = { w = w, h = h },
        velocity         = { x = vx, y = vy },
        renderable       = { r = 255, g = 0, b = 0, a = 255 },
        sprite           = { texture = tex, col = 0, row = 0,
                             w = sprite.w, h = sprite.h,
                             offset_x = sprite.ox, offset_y = sprite.oy },
        animation        = anim,
        enemy_projectile = {},
    })
end

-- Head Weapon  →n 4 frames 24x24
local HEAD_SPR  = { w = 24, h = 24, ox = 3, oy = 12 }
local HEAD_ANIM = { frames = 4, cols = 4, fps = 12 }
local HEAD_W, HEAD_H = 24 * 2, 24 * 2
WEAPONS.boss_head = {
    interval = 0,
    fire = function(x, y)
        for _, vy in ipairs({ -120, 0, 120 }) do
            boss_bullet("assets/01_Boss/BossHeadProjectile.gif",
                        x - HEAD_W, y - HEAD_H / 2, -350, vy, HEAD_W, HEAD_H,
                        HEAD_SPR, HEAD_ANIM)
        end
    end,
}

-- Mid mob Weapon → 9 frames  18x18
local MID_SPR  = { w = 18, h = 18, ox = 7, oy = 1 }
local MID_ANIM = { frames = 9, cols = 9, fps = 14 }
local MID_W, MID_H = 18 * 2, 18 * 2
WEAPONS.boss_middle = {
    interval = 0,
    fire = function(x, y)
        boss_bullet("assets/01_Boss/MiddleEnemyProjectile.gif",
                    x - MID_W, y - MID_H / 2, -450, 0, MID_W, MID_H,
                    MID_SPR, MID_ANIM)
    end,
}
