player = spawn({
    name         = "Player",
    position     = { x = 100, y = SCREEN_H / 2 },
    hitbox       = { w = 60, h = 30 },
    velocity     = {},
    renderable   = { r = 0, g = 200, b = 255 },
    controllable = { speed = 400 },
    input        = {},
    shooter      = { interval = 0.2, weapon = "basic"},
    sprite       = { texture = "assets/ship.gif", col = 1, row = 1, w = 166 / 5, h = 86 / 5 },
})
