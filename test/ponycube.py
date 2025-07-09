# 姿态显示

import pygame
from euclid import Quaternion

class Screen:
    def __init__(self, x=320, y=280, scale=1):
        self.i = pygame.display.set_mode((x, y))
        self.originx = self.i.get_width() / 2
        self.originy = self.i.get_height() / 2
        self.scale = scale

    def project(self, v):
        x = v[0] * self.scale + self.originx
        y = v[1] * self.scale + self.originy
        return (x, y)

    def depth(self, v):
        return v[2]


class Cube:
    def __init__(self, a=10, b=10, c=10):
        self.a = a
        self.b = b
        self.c = c
        self.pts = [
            [-a, b, c], [a, b, c], [a, -b, c], [-a, -b, c],
            [-a, b, -c], [a, b, -c], [a, -b, -c], [-a, -b, -c],
        ]

    def origin(self):
        a, b, c = self.a, self.b, self.c
        self.pts = [
            [-a, b, c], [a, b, c], [a, -b, c], [-a, -b, c],
            [-a, b, -c], [a, b, -c], [a, -b, -c], [-a, -b, -c],
        ]

    def draw(self, screen, q):
        self.origin()
        R = q.get_matrix()
        self.pts = [[sum(R[i][j] * p[j] for j in range(3)) for i in range(3)] for p in self.pts]

        edges = [
            (0, 1), (1, 2), (2, 3), (3, 0),
            (4, 5), (5, 6), (6, 7), (7, 4),
            (0, 4), (1, 5), (2, 6), (3, 7),
        ]
        for edge in edges:
            pygame.draw.line(screen.i, (255, 255, 255), screen.project(self.pts[edge[0]]), screen.project(self.pts[edge[1]]))

    def erase(self, screen):
        screen.i.fill((0, 0, 0))