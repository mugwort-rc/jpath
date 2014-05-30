from __jpath import *

__path_repr = lambda self: str([x for x in self])

jpath.__repr__ = __path_repr
expr_path.__repr__ = __path_repr

