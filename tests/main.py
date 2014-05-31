import unittest

import jpath

class TestJPath(unittest.TestCase):
    def test_parse(self):
        obj = jpath.parse('"path"/"to"/"resource"["item"="hoge"]')
        self.assertEqual(len(obj), 3)

        item = obj[0]
        self.assertEqual(item.path_index, 'path')
        self.assertEqual(item.cond, None)

        item = obj[2]
        self.assertEqual(item.path_index, 'resource')
        self.assertEqual(item.cond.type, jpath.cond_node.VARIABLE)
        key, value = item.cond.value
        self.assertEqual(len(key), 1)
        self.assertEqual(key[0], 'item')
        self.assertEqual(value, 'hoge')

    def test_json_value(self):
        testdata = [
            ('"str"["value"="str"]', 'str'),
            ('"int"["value"=123]', 123),
            ('"bool"["value"=true]', True),
            ('"bool"["value"=false]', False),
            ('"null"["value"=null]', None),
        ]

        for source, value in testdata:
            obj = jpath.parse(source)
            self.assertEqual(len(obj), 1)
            self.assertTrue(obj[0].cond is not None)
            self.assertEqual(obj[0].cond.type, jpath.cond_node.VARIABLE)
            self.assertEqual(len(obj[0].cond.value), 2)
            self.assertEqual(obj[0].cond.value[0][0], 'value')
            self.assertEqual(obj[0].cond.value[1], value)

    def test_cond_novalue(self):
        obj = jpath.parse('"novalue"["value"]')
        self.assertEqual(len(obj), 1)
        self.assertTrue(obj[0].cond is not None)
        self.assertEqual(obj[0].cond.type, jpath.cond_node.VARIABLE)
        self.assertEqual(len(obj[0].cond.value), 1)
        self.assertEqual(obj[0].cond.value[0][0], 'value')

    def test_escape_quoted_string(self):
        obj = jpath.parse(r'"test\"quoted\"string"')
        self.assertEqual(len(obj), 1)
        self.assertEqual(obj[0].path_index, 'test"quoted"string')
        self.assertEqual(repr(obj[0]), r'"test\"quoted\"string"')

    def _assert_cond_tree(self, obj):
        left = obj[0].cond.left
        right = obj[0].cond.right
        self.assertEqual(len(left.value), 2)
        self.assertEqual(len(right.value), 2)
        self.assertEqual(len(left.value[0]), 1)
        self.assertEqual(len(right.value[0]), 1)
        self.assertEqual(left.value[0][0], 'item')
        self.assertEqual(right.value[0][0], 'hoge')
        self.assertEqual(left.value[1], 'value')
        self.assertEqual(right.value[1], 'value')

    def test_cond_tree_and(self):
        obj = jpath.parse('"and"["item"="value" and "hoge"="value"]')
        self.assertEqual(len(obj), 1)
        self.assertTrue(obj[0].cond is not None)
        self.assertEqual(obj[0].cond.type, jpath.cond_node.OPERATOR)
        self.assertEqual(obj[0].cond.operator, jpath.cond_operator_node.AND)
        self._assert_cond_tree(obj)

    def test_cond_tree_or(self):
        obj = jpath.parse('"or"["item"="value" or "hoge"="value"]')
        self.assertEqual(len(obj), 1)
        self.assertTrue(obj[0].cond is not None)
        self.assertEqual(obj[0].cond.type, jpath.cond_node.OPERATOR)
        self.assertEqual(obj[0].cond.operator, jpath.cond_operator_node.OR)
        self._assert_cond_tree(obj)

    def test_cond_tree_parenthesis(self):
        obj = jpath.parse('"test"[(1=1 or 2=2) and 3=3]')
        self.assertEqual(len(obj), 1)
        self.assertTrue(obj[0].cond is not None)
        self.assertEqual(obj[0].cond.type, jpath.cond_node.OPERATOR)
        self.assertEqual(obj[0].cond.operator, jpath.cond_operator_node.AND)

        left = obj[0].cond.left
        right = obj[0].cond.right
        self.assertEqual(left.type, jpath.cond_node.OPERATOR)
        self.assertEqual(left.operator, jpath.cond_operator_node.OR)

def main():
    unittest.main()

if __name__ == '__main__':
    main()

