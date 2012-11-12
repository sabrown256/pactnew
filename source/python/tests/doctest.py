#
# Test code in docuement
#


import pact_test.pdb as pdb

d = pdb.pdbdata(4)
print(d)

d = pdb.pdbdata(4, 'double')
print(d)

r = pdb.unpack(d)
print(type(r))

d = pdb.pdbdata((4,5.))
print(d)

r = pdb.unpack(d)
print(type(r))

pdb.setform(array=pdb.AS_TUPLE)

r = pdb.unpack(d)
print(type(r))
print(r)

v = [None, [4., 5.]]
print(pdb.vif.get_obj_descr(v))
#d = pdb.pdbdata([None, [4., 5.]])
d = pdb.pdbdata([None, [4., 5.]], 'double **')
print(d)

r = pdb.unpack(d)
print(r)

print(r[0])
print(r[1])



######################################

fp = pdb.open("xxx", "w")
print(type(fp))
pdb.files

ref = [2.0, 3.0, 4.0, 5.0]
fp.write("d2", ref, ind=[2,2])
fp.write("d3", ref, "float")
fp.close()

print("######################################")

class User:
    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c
    def __repr__(self):
        return 'User(%(a)d, %(b)d, %(c)f)' % self.__dict__

def makeUser(dict):
    return User(dict['a'], dict['b'], dict['c'])

fp = pdb.open("user.pdb", "w")
fp.defstr( "user", ("int a", "int b", "float c"))
fp.register_class(User, "user", makeUser)

v1 = User(1,2,3)
fp.write("var1", v1)
v2 = fp.read("var1")
print("v1 =", v1)
print("v2 =", v2)
fp.close()

