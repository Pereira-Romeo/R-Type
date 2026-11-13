local BOSS_W, BOSS_H = 162, 213.6
local BOSS_SCALE = 1.5
local BOSS_DELAY = 2
local elapsed, boss_spawned = 0, false

local function Rec(x, y, w, h)
    return { x = x * BOSS_SCALE, y = y * BOSS_SCALE, w = w * BOSS_SCALE, h = h * BOSS_SCALE }
end

local function spawn_boss()
    local w, h = BOSS_W * BOSS_SCALE, BOSS_H * BOSS_SCALE
    local y = (SCREEN_H - h) / 2
    return spawn({
        name       = "Orange Boss",
        position   = { x = SCREEN_W, y = y },
        stat       = { life = 1500, lifeMax = 1500 },
        hitbox     = { w = w, h = h },
        hurtbox    = { x = 80 * BOSS_SCALE, y = 80 * BOSS_SCALE,
                       w = 60 * BOSS_SCALE, h = 80 * BOSS_SCALE },
        renderable = { r = 255, g = 0, b = 0, a = 255 },
        sprite     = { texture = "assets/01_Boss/BossOrange.gif",
                       col = 0, row = 0, w = BOSS_W, h = BOSS_H,
                       offset_x = 24, offset_y = 0 },
        animation  = { frames = 36, cols = 4, fps = 8, loop = true,
                       loop_from = 32, loop_to = 35 },
        boss_motion = { stop_x = SCREEN_W - w - 40, enter_speed = 120,
                        base_y = y, amp = 100, speed = 1.2 },
        enemy      = {},
        boss_shooter = {
            active_from = 32,
            shooters = {
                { weapon = "boss_head", x = 5  * BOSS_SCALE, y = 60  * BOSS_SCALE, interval = 2.0, delay = 0.5 },
                { weapon = "boss_middle", x = 80 * BOSS_SCALE, y = 120 * BOSS_SCALE, interval = 0.8 },
            },
        },
        solids = {
            Rec(3, 25, 55, 65),
            Rec(20, 0, 110, 60),
            Rec(100, 40, 57, 130),
            Rec(55, 165, 102, 40),
        },

    })
end

on_update(function(dt)
    elapsed = elapsed + dt;
    if not boss_spawned and elapsed >= BOSS_DELAY then
        boss_spawned = true
        spawn_boss()
    end
end)

