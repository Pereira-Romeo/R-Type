local layers = {
    { count = 80, speed = 20,  size = 1, brightness = 90  },
    { count = 50, speed = 60,  size = 1, brightness = 170 },
    { count = 25, speed = 140, size = 2, brightness = 255 },
}

for _, l in ipairs(layers) do
    l.stars = {}
    for i = 1, l.count do
        l.stars[i] = { x = math.random() * SCREEN_W, y = math.random() * SCREEN_H }
    end
end

on_update(function(dt)
    for _, l in ipairs(layers) do
        local dx = l.speed * dt
        for _, s in ipairs(l.stars) do
            s.x = s.x - dx
            if s.x < -5 then
                s.x = SCREEN_W + math.random() * 20
                s.y = math.random() * SCREEN_H
            end
        end
    end
end)

on_draw(function()
    for _, l in ipairs(layers) do
        set_color(l.brightness, l.brightness, l.brightness)
        for _, s in ipairs(l.stars) do
            if l.size <= 1 then draw_point(s.x, s.y)
            else fill_rect(s.x, s.y, l.size, l.size) end
        end
    end
end, -100)
