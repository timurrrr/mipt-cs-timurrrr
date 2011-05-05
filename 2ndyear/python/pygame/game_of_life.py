import math
import pygame
import pygame.locals
import random
import sys

def ProcessInputEvents(events):
  # http://stackoverflow.com/questions/423379/global-variables-in-python
  global should_exit
  global running

  for e in events:
    if e.type == pygame.locals.QUIT:
      should_exit = True
    if e.type == pygame.KEYDOWN and e.key == 32:  # space
      running = True
    if e.type == pygame.KEYUP and e.key == 32:  # space
      running = False

class GameOfLife:
  def __init__(self):
    self.field = [[]] * 64
    for x in range(64):
      self.field[x] = [[]] * 48
      for y in range(48):
        self.field[x][y] = random.uniform(0,1) > 0.8

  def Tick(self):
    new_field = [[]] * 64
    for x in range(64):
      new_field[x] = [[]] * 48
      for y in range(48):
        num_neighbors = 0
        for dx in [-1, 0, 1]:
          for dy in [-1, 0, 1]:
            if dx == 0 and dy == 0:
              continue
            if self.field[(x+dx) % 64][(y+dy) % 48]:
              num_neighbors += 1
        new_field[x][y] = False
        if self.field[x][y]:
          # live cell
          if num_neighbors in [2,3]:
            new_field[x][y] = True
        else:
          # dead cell
          if num_neighbors == 3:
            new_field[x][y] = True

    self.field = new_field

  def Render(self):
    screen.fill((0,0,0))
    for x in range(len(self.field)):
      for y in range(len(self.field[x])):
        if self.field[x][y]:
          color = (255,255,255)
        else:
          color = (64,64,64)
        pygame.draw.rect(screen, color,
                         (x*10 + 1, y*10 + 1, 8, 8))

    pygame.display.flip()


pygame.init()
screen = pygame.display.set_mode((640, 480))
world = GameOfLife()

clock = pygame.time.Clock()
TARGET_FPS=4
should_exit = False
running = False
while not should_exit:
  world.Render()
  clock.tick(TARGET_FPS)
  ProcessInputEvents(pygame.event.get())
  if running:
    world.Tick()
