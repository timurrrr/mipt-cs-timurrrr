import math
import pygame
import pygame.locals
import random
import sys

pygame.init()
screen = pygame.display.set_mode((640, 480))

should_exit = False

def ProcessInputEvents(events):
  global should_exit

  for e in events:
    if e.type == pygame.locals.QUIT:
      should_exit = True

def VectorMultiplyAdd(a, b, c=1.0):
  """ returns a + c*b """
  assert len(a) == len(b)

  ret = a[:]
  for index in range(len(a)):
    ret[index] += c * b[index]
  return ret

def IntegrateEuler(state, derivative_function, t, delta_t):
  derivative = derivative_function(t, state)
  return VectorMultiplyAdd(state, derivative, delta_t)

def IntegrateRungeKutta(state, derivative_function, t, delta_t):
  # See http://en.wikipedia.org/wiki/Runge-Kutta_method
  # for the definition of k1 ... k4.
  # s1 ... s4 are just state parameters used for calculating k1 ... k4.
  s1 = state
  k1 = derivative_function(t, s1)

  s2 = VectorMultiplyAdd(state, k1, delta_t/2)
  k2 = derivative_function(t + delta_t/2, s2)

  s3 = VectorMultiplyAdd(state, k2, delta_t/2)
  k3 = derivative_function(t + delta_t/2, s3)

  s4 = VectorMultiplyAdd(state, k2, delta_t)
  k4 = derivative_function(t + delta_t, s4)

  sum_k = VectorMultiplyAdd(
            VectorMultiplyAdd(k1, k2, 2),
            VectorMultiplyAdd(k4, k3, 2)
          )
  return VectorMultiplyAdd(state, sum_k, delta_t/6)

def SpringForce(t, state):
  K = 3
  x = state[0]
  vx = state[1]
  return [vx, -K*(x-320)]

clock = pygame.time.Clock()
TARGET_FPS=100
state = [50, 0]
time = 0
while not should_exit:
  delta_t = 0.001 * clock.tick(TARGET_FPS)  # returns actual delta in ms.
  ProcessInputEvents(pygame.event.get())

  #state = IntegrateEuler(state, SpringForce, time, delta_t)
  state = IntegrateRungeKutta(state, SpringForce, time, delta_t)
  time += delta_t

  screen.fill((0,0,0))
  # Draw reference lines
  pygame.draw.line(screen, (128,128,128), (0,240), (640,240))
  pygame.draw.line(screen, (128,128,128), (50,230), (50,250))
  pygame.draw.line(screen, (128,128,128), (320,230), (320,250))
  pygame.draw.line(screen, (128,128,128), (590,230), (590,250))

  pos = (int(state[0]), 240)
  pygame.draw.circle(screen, (255,255,255), pos, 5)
  pygame.display.flip()
