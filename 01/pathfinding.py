#!/usr/bin/python3
import time
import argparse
from dataclasses import dataclass
from random import randint, seed
from collections import deque
from heapq import heappush, heappop
from dataclasses import field
from math import sqrt

Node = tuple[int, int]

@dataclass(frozen=True)
class ResultDictionary:
    """Class representing result of pathfinding algorithm"""
    distance : dict[Node, int]
    path : list[Node]

EMPTY_RESULT = ResultDictionary({}, [])

class Maze:

    def __init__(self, file_name:str, wall_char:str='X', empty_char:str=' '):
        self.maze = []
        self.height = 0
        self.width  =  0
        self.start  = None
        self.end    = None
        self._read_maze(file_name, wall_char, empty_char)

    def _read_maze(self, filename : str, wall_char:str, empty_char:str):
        with open(filename) as f:
            for line in f:
                match line[0]:
                    case 's':
                        self.start = eval('(' + line[6:] + ')') # LISP cursed
                    case 'e':
                        self.end = eval('(' + line[4:] + ')')
                    case _:
                        self.maze.append(list(map(lambda c: c == empty_char, line)))
        self.height = len(self.maze)
        self.width = len(self.maze[0])

    def neighbors(self, n:Node):
        for dx, dy in [(-1, 0), (0, -1), (1, 0), (0, 1)]:
            if self.maze[n[1] + dy][n[0] + dx]:
                yield (n[0] + dx, n[1] + dy)

    def display(self, result:ResultDictionary=EMPTY_RESULT):
        path_set = set(result.path)
        for i in range(self.height):
            for j in range(self.width):
                if not self.maze[i][j]:
                    print('█', end='')
                elif (j, i) == self.start:
                    print('S', end='')
                elif (j, i) == self.end:
                    print('E', end='')
                elif (j, i) in path_set:
                    print('+', end='')
                elif (j, i) in result.distance:
                    print('.', end='')
                else:
                    print(' ', end='')
            print('')
        if result.distance:
            print('Cells opened: ', len(result.distance))
            if result.path:
                print('Path length: ', len(result.path) - 1)
            else:
                print('End is unreacheable from start')

def make_result(maze:Maze, distance:dict[Node, int], pred:dict[Node, Node]):
    path = [maze.end]
    while path[-1] != maze.start:
        path.append(pred[path[-1]])
    return ResultDictionary(distance, path[::-1])


def RandomSearch(maze : Maze) -> ResultDictionary:
    distance = {maze.start:0}
    pred = {maze.start:maze.start}
    tovisit = [maze.start]
    if maze.start == maze.end:
        return ResultDictionary(distance, tovisit)

    while tovisit:
        idx = randint(0, len(tovisit) - 1)
        tovisit[idx], tovisit[-1] = tovisit[-1], tovisit[idx]
        current = tovisit.pop()

        for n in maze.neighbors(current):
            if n not in distance:
                tovisit.append(n)
                pred[n] = current
                distance[n] = distance[current] + 1
                if n == maze.end:
                    return make_result(maze,  distance, pred)
    return ResultDictionary(distance, [])

def BFS(maze : Maze) -> ResultDictionary:
    distance = {maze.start:0}
    pred = {maze.start:maze.start}
    tovisit = deque([maze.start])
    if maze.start == maze.end:
        return ResultDictionary(distance, tovisit)

    while tovisit:
        current = tovisit.popleft()

        for n in maze.neighbors(current):
            if n not in distance:
                tovisit.append(n)
                pred[n] = current
                distance[n] = distance[current] + 1
                if n == maze.end:
                    return make_result(maze,  distance, pred)
    return ResultDictionary(distance, [])

def DFS(maze:Maze) -> ResultDictionary:
    distance = {maze.start:0}
    pred = {maze.start:maze.start}
    stack = [maze.start]
    if maze.start == maze.end:
        return ResultDictionary(distance, stack)

    while stack:
        current = stack[-1]
        do_pop = True

        for n in maze.neighbors(current):
            if n not in distance:
                do_pop = False
                stack.append(n)
                pred[n] = current
                distance[n] = distance[current] + 1
                if n == maze.end:
                    return make_result(maze,  distance, pred)
        if do_pop:
            stack.pop()
    return ResultDictionary(distance, [])

@dataclass(order=True)
class PrioritizedNode:
    node: Node=field(compare=False)
    priority: float|int = 0.0
    distance:int=field(compare=False, default=0)

    def __iter__(self):
        return iter((self.node, self.distance))

def L1_dist(maze : Maze):
    def l1(n : Node):
        return abs(n[0] - maze.end[0]) + abs(n[1] - maze.end[1])
    return l1

def L2_dist(maze : Maze):
    def l2(n : Node):
        return sqrt((n[0] - maze.end[0])**2 + (n[1] - maze.end[1])**2)
    return l2

def Chebyshev_dist(maze : Maze):
    def chebyshev(n : Node):
        return max((n[0] - maze.end[0]), abs(n[1] - maze.end[1]))
    return chebyshev

def GreedySearch(maze:Maze, heuristic) -> ResultDictionary:
    distance = {maze.start:0}
    pred = {maze.start:maze.start}
    heap = [PrioritizedNode(maze.start)]
    if maze.start == maze.end:
        return ResultDictionary(distance, [maze.start])

    while heap:
        current = heappop(heap).node
        for n in maze.neighbors(current):
            if n not in distance:
                pred[n] = current
                distance[n] = distance[current] + 1
                heappush(heap, PrioritizedNode(n, priority=heuristic(n)))
                if n == maze.end:
                    return make_result(maze,  distance, pred)
    return ResultDictionary(distance, [])

def Astar(maze:Maze, heuristic) -> ResultDictionary:
    distance = {maze.start:0}
    pred = {maze.start:maze.start}
    heap = [PrioritizedNode(maze.start)]
    if maze.start == maze.end:
        return ResultDictionary(distance, [maze.start])

    while heap:
        current, current_distance = heappop(heap)
        if current_distance > distance[current]:
            continue
        for n in maze.neighbors(current):
            if n not in distance or distance[n] > distance[current] + 1:
                pred[n] = current
                distance[n] = distance[current] + 1
                heappush(heap, PrioritizedNode(n, distance=distance[n], priority=heuristic(n)+distance[n]))
                if n == maze.end:
                    return make_result(maze,  distance, pred)
    return ResultDictionary(distance, [])

def TrivialHeuristic(*_):
    return lambda _: 0

def OptimalHeuristic(maze : Maze):
    distance = {maze.end:0}
    tovisit = deque([maze.end])

    while tovisit:
        current = tovisit.popleft()
        for n in maze.neighbors(current):
            if n not in distance:
                tovisit.append(n)
                distance[n] = distance[current] + 1
    return lambda n : distance[n]

def display_uninformed(maze : Maze):
    for algo, name in zip([RandomSearch, BFS, DFS], ['Random Search:', 'BFS:          ', 'DFS:          ']):
        start = time.time()
        res = algo(maze)
        end = time.time()
        print(f"{name} Opened: {len(res.distance):<7}  Path: {len(res.path) - 1:<4}  Time: {1000*(end-start):.2f}ms")

def display_informed(maze : Maze):
    for heur, name in zip([Chebyshev_dist, L2_dist, L1_dist], ['L_inf:', 'L2:   ', 'L1:   ']):
        start = time.time()
        res = GreedySearch(maze, heur(maze))
        end = time.time()
        print(f"Greedy Search with {name} Opened: {len(res.distance):<7}  Path: {len(res.path) - 1:<4}  Time: {1000*(end-start):.2f}ms")

    print('')
    for heur, name in zip([Chebyshev_dist, L2_dist, L1_dist], ['L_inf:', 'L2:   ', 'L1:   ']):
        start = time.time()
        res = Astar(maze, heur(maze))
        end = time.time()
        print(f"A* with {name}            Opened: {len(res.distance):<7}  Path: {len(res.path) - 1:<4}  Time: {1000*(end-start):.2f}ms")

    print('')
    for heur, name in zip([TrivialHeuristic, OptimalHeuristic], ['trivial heuristic:', 'optimal heuristic:']):
        start = time.time()
        res = GreedySearch(maze, heur(maze))
        end = time.time()
        print(f"Greedy Search with {name} Opened: {len(res.distance):<7}  Path: {len(res.path) - 1:<4}  Time: {1000*(end-start):.2f}ms")
    print('')
    for heur, name in zip([TrivialHeuristic, OptimalHeuristic], ['trivial heuristic:', 'optimal heuristic:']):
        start = time.time()
        res = Astar(maze, heur(maze))
        end = time.time()
        print(f"A* with {name}            Opened: {len(res.distance):<7}  Path: {len(res.path) - 1:<4}  Time: {1000*(end-start):.2f}ms")

def display_algos(maze : Maze):
    display_uninformed(maze)
    print('')
    display_informed(maze)


parser = argparse.ArgumentParser()
parser.add_argument('-f', '--file', help='File containing maze')
parser.add_argument('--algo',       choices=['random', 'bfs', 'dfs', 'greedy', 'astar'], default='astar', help='Choose algortithm to find path. Default: astar')
parser.add_argument('--heuristic',  choices=['l1', 'l2', 'linf', '0', 'optimal'], help='Choose heuristic for Gready Best First Search and A*. Default: l1')
parser.add_argument('--seed',       type=int, default=0, help='Seed for Random Search')
parser.add_argument('--legend',     action='store_true', help='Show maze map legend')
parser.add_argument('--runall',     action='store_true', help='Run all algorithms and display measures')

def heuristic(name, maze):
    match name:
        case None | 'l1':
            return L1_dist(maze)
        case 'l2':
            return L2_dist(maze)
        case 'linf':
            return Chebyshev_dist(maze)
        case '0':
            return TrivialHeuristic()
        case 'optimal':
            return OptimalHeuristic(maze)
        case _:
            raise AttributeError('Unknown heuristic')

if __name__ == '__main__':
    args = parser.parse_args()
    if args.legend:
        print(
            """
    '█' - Wall cell
    'S' - Start of path
    'E' - End of path
    '+' - Cell included in the path
    '.' - Visited cell that was not included in a path
    ' ' - Empty cell
            """
        )
        exit(0)

    if not args.file:
        raise AttributeError('Filename (-f/--file) not specified.')

    maze = Maze(args.file)

    if args.runall:
        display_algos(maze)
        exit(0)

    match args.algo:
        case 'random':
            if args.heuristic:
                raise AttributeError('Random Search does not use heuristic.')
            seed(args.seed)
            maze.display(RandomSearch(maze))
        case 'bfs':
            if args.heuristic:
                raise AttributeError('BFS does not use heuristic.')
            maze.display(BFS(maze))
        case 'dfs':
            if args.heuristic:
                raise AttributeError('DFS does not use heuristic.')
            maze.display(DFS(maze))
        case 'greedy':
            maze.display(GreedySearch(maze, heuristic(args.heuristic, maze)))
        case 'astar':
            maze.display(Astar(maze, heuristic(args.heuristic, maze)))
