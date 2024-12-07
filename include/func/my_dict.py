# from typing import Mapping, OrderedDict


from collections.abc import Mapping
from typing import MutableMapping

dict0: dict = {
    'name': 'John',
    "age": 25,
}
dict0['name']
dict0.get('name')

age: int = dict0['age']
print(dict0.keys())
print(dir(dict0))
Mapping1 = MutableMapping()
# MyDict1 = MyDict() # err: Can't instantiate abstract class Mapping without an implementation for abstract methods '__getitem__', '__iter__', '__len__'
# class MyDict(Mapping):
#     def __init__(self: dict[str, _VT]):
#         pass
#     def __getitem__(self, key):
#         pass
#     def __iter__(self):
#         pass
#     def __len__(self):
#         pass

# Mapping1 = MyDict()
# print(Mapping1)
# Mapping1["name"] = "John" # err: 'MyDict' object does not support item assignment
# MyDict1:str = MyDict()
# print(Mapping1)
# print(Mapping1.__repr__())
# print(Mapping1.__str__())
# print(MyDict1)
# print(MyDict1.__str__())