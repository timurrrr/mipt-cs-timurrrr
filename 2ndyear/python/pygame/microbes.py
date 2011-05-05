import math
import pygame
import pygame.locals
import random
import sys

def ProcessInputEvents(events):
  # http://stackoverflow.com/questions/423379/global-variables-in-python
  global should_exit

  for e in events:
    if e.type == pygame.locals.QUIT:
      should_exit = True
    elif e.type == pygame.MOUSEBUTTONDOWN:
      world.SpawnFood(e.pos)
    else:
      print e

def RandomXY():
  x = random.randint(0,640)
  y = random.randint(0,480)
  return (x,y)

def GetDistance(p1, p2):
  (x1,y1) = p1
  (x2,y2) = p2
  return math.sqrt(math.pow(x1-x2, 2) + math.pow(y1-y2, 2))

class Food:
  def __init__(self):
    self.position = RandomXY()

  def Render(self, surface):
    pygame.draw.circle(surface, (192,192,192), self.position, 1)

class Microbe:
  def __init__(self):
    self.position = RandomXY()
    self.direction = random.uniform(0, 2 * math.pi)
    self.size = 2
    self.color = random.choice([
        (255, 0, 0),  # red
        (0, 255, 0),  # green
        (0, 0, 255),  # blue
        (255, 255, 0),  # yellow
    ])

  def Render(self, surface):
    pygame.draw.circle(surface, self.color, self.position, self.size)

  def GetControl(self, world_state):
    nearest_f = None
    nearest_dist = -1
    for f in world_state.food:
      curr_dist = GetDistance(f.position, self.position)
      if not nearest_f or curr_dist < nearest_dist:
        nearest_f = f
        nearest_dist = curr_dist
    if not nearest_f or nearest_dist > 100:
      return random.uniform(-1, 1)

    (f_x, f_y) = nearest_f.position
    (s_x, s_y) = self.position
    target_direction = math.atan2(f_y-s_y, f_x-s_x)
    if (target_direction - self.direction) % (2 * math.pi) < math.pi:
      return 1
    return -1

  def Update(self, control, delta_t):
    ROTATION = 0.001
    self.direction += ROTATION * control * delta_t
    self.direction = self.direction % (2 * math.pi)
    (x,y) = self.position
    SPEED = 0.03
    x += SPEED * math.cos(self.direction) * delta_t
    y += SPEED * math.sin(self.direction) * delta_t
    x = x % 640
    y = y % 480
    self.position = (x,y)

class GameWorld:
  def __init__(self):
    self.current_time = 0

    self.microbes = []
    for i in range(0, 20):
      self.microbes.append(Microbe())

    self.food = [ ]
    for i in range(0, 10):
      self.food.append(Food())

  def Render(self):
    screen.fill((0,0,0))
    for obj in self.food + self.microbes:
      obj.Render(screen)
    pygame.display.flip()

  def GetAiControls(self):
    controls = {}
    for m in self.microbes:
      controls[m] = m.GetControl(self)
    return controls

  def SpawnFood(self, position):
    new_f = Food()
    new_f.position = position
    self.food.append(new_f)

  def Update(self, controls, delta_t):
    for m in self.microbes:
      m.Update(controls[m], delta_t)

    for m in self.microbes:
      for f in self.food:
        if GetDistance(f.position, m.position) < 5:
          # Eat the food
          self.food.remove(f)
          self.food.append(Food())
          m.size += 1
          if m.size > 5:
            old_dir = m.direction
            new_m = Microbe()
            new_m.position = m.position
            new_m.color = m.color
            new_m.direction = old_dir + 1
            m.direction = old_dir - 1
            m.size = new_m.size
            self.microbes.append(new_m)
    self.current_time += delta_t


pygame.init()
screen = pygame.display.set_mode((640, 480))
world = GameWorld()

clock = pygame.time.Clock()
TARGET_FPS=4
should_exit = False
while not should_exit:
  delta_t = clock.tick(TARGET_FPS)  # returns actual delta in ms.
  ProcessInputEvents(pygame.event.get())
  controls = world.GetAiControls()
  world.Update(controls, delta_t)
  world.Render()
