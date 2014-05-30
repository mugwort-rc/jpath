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
        key, value = item.cond
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
            self.assertEqual(len(obj[0].cond), 2)
            self.assertEqual(obj[0].cond[1], value)

    def test_cond_novalue(self):
        obj = jpath.parse('"novalue"["value"]')
        self.assertEqual(len(obj), 1)
        self.assertTrue(obj[0].cond is not None)
        self.assertEqual(len(obj[0].cond), 1)
        self.assertEqual(len(obj[0].cond[0]), 1)
        self.assertEqual(obj[0].cond[0][0], 'value')

def main():
    unittest.main()

if __name__ == '__main__':
    main()

