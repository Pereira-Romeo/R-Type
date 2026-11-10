monster = {
  sprite = "content/sprites/snake.png",
  hp     = 3,
  speed  = 140,
  width  = 32,
  height = 32,
}

function on_spawn(self)
  self.data.base_y = self.y
  self.data.age = 0
end

function update(self, dt)
  self.data.age = self.data.age + dt
  self.x = self.x - monster.speed * dt
  self.y = self.data.base_y + math.sin(self.data.age * 3) * 60
end
