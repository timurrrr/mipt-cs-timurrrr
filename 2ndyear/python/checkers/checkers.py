#!/bin/python

import copy
import re

CHECKER_SIZE=4
# CHECKER_SIZE=4 case:
#x=0123
# " B B"   y=0
# ". . "
# " . ."
# "W W "   y=3

class BoardState:
    def __init__(self):
        self.__state = []   # __state[y][x] contains "." or "W" or "B"
        for x in range(0, CHECKER_SIZE):
            self.__state.append([])
        for line_id in range(0, CHECKER_SIZE/2 - 1):
            if line_id % 2 == 0:
                black_line = " B" * (CHECKER_SIZE/2)
                white_line = "W " * (CHECKER_SIZE/2)
            else:
                black_line = "B " * (CHECKER_SIZE/2)
                white_line = " W" * (CHECKER_SIZE/2)
            self.__state[line_id]  = list(black_line)
            self.__state[-1-line_id] = list(white_line)
        for line_id in range(CHECKER_SIZE/2 - 1, CHECKER_SIZE/2 + 1):
            if line_id % 2 == 0:
                empty_line = " ." * (CHECKER_SIZE/2)
            else:
                empty_line = ". " * (CHECKER_SIZE/2)
            self.__state[line_id] = list(empty_line)

    def __str__(self):
        # Return a human-readable form
        ret = ""
        for line_id in range(0, CHECKER_SIZE):
            if line_id != 0:
                ret += "\n"
            ret += "\"" + "".join(self.__state[line_id]) + "\""
        return ret

    def SetState(self, state):
        """ state = Board state serialized to string """
        # Sanity check
        assert len(state) == CHECKER_SIZE * CHECKER_SIZE
        assert state.count("W") <= CHECKER_SIZE / 2 * (CHECKER_SIZE/2 - 1)
        assert state.count("B") <= CHECKER_SIZE / 2 * (CHECKER_SIZE/2 - 1)

        # Create a sanity regexp
        regexp = (" [.WB]" * (CHECKER_SIZE/2) +
                  "[.WB] " * (CHECKER_SIZE/2)) * (CHECKER_SIZE/2)
        assert re.match(regexp, state)

        self.__state = []
        for line_id in range(0, CHECKER_SIZE):
            line = state[line_id*CHECKER_SIZE : (line_id+1)*CHECKER_SIZE]
            self.__state.append(list(line))

    def __ValidCell(self, x, y):
        return x >= 0 and x < CHECKER_SIZE and y >= 0 and y < CHECKER_SIZE and \
               (x+y) % 2 == 1

    def __Go(self, cur_player, from_x, from_y, d_x, d_y):
        """ Returns False if can't
            or makes a move and return True
        """
        if cur_player != self.__state[from_y][from_x]:
            return False
        assert cur_player in set(["W", "B"])
        assert d_x * d_x == 1
        assert d_y * d_y == 1
        to_x = from_x + d_x
        to_y = from_y + d_y
        if not self.__ValidCell(to_x, to_y):
            return False
        dest_cell = self.__state[to_y][to_x]
        assert dest_cell in set([".", "W", "B"])
        if dest_cell == cur_player:
            return False
        elif dest_cell == ".":
            if cur_player == "W" and d_y == 1:
                return False
            elif cur_player == "B" and d_y == -1:
                return False
            else:
                self.__state[from_y][from_x] = "."
                self.__state[to_y][to_x] = cur_player
                return True
        else:
            # Enemy there!
            if not self.__ValidCell(to_x + d_x, to_y + d_y):
                return False
            jump_cell = self.__state[to_y + d_y][to_x + d_x]
            if jump_cell != ".":
                return False
            return self.__Eat(cur_player, from_x, from_y, d_x * 2, d_y * 2)

    def __Eat(self, cur_player, from_x, from_y, d_x, d_y):
        """ Returns False if can't
            or makes a move and return True
        """
        assert cur_player in set(["W", "B"])
        assert d_x * d_x == 4
        assert d_y * d_y == 4
        assert self.__state[from_y][from_x] in set([".", cur_player])
        # TODO! Hint: should be recursive
        assert False, "NOT IMPLEMENTED"


    def GenerateStepSet(self, cur_player):
        """ cur_player = W or B """
        ret = []
        for x in range(0, CHECKER_SIZE):
            for y in range(0, CHECKER_SIZE):
                if not self.__ValidCell(x, y):
                    continue
                for d_x in [-1, 1]:
                    for d_y in [-1, 1]:
                        new_state = copy.deepcopy(self)
                        try_to_go = new_state.__Go(cur_player, x, y, d_x, d_y)
                        if try_to_go:
                            ret.append(new_state)
        return set(ret)


def Demo():
    board = BoardState()
    print board
    print "====================="

    board.SetState(
        " . B"
        ". B "
        " W ."
        ". W "
    )
    print board
    print "====================="

    board = BoardState()
    steps = board.GenerateStepSet("W")
    for variant in steps:
        print "\n******\n%s\n******\n" % variant

Demo()
