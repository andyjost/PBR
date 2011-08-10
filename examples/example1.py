import example1

print "This script demonstrates the examples from the wiki Introduction page."
print "https://github.com/andyjost/PBR/wiki/Introduction"
print ""

print "------ foreach example ------"
lst = [1, 2, 3, 'hello', {'Frodo':'hobbit', 'Gandalf':'wizard'}, [1,2,3,5,7,11,13]]
example1.foreach(lst)


print "\n\n"
print "------ iterate over the keys in a dictionary ------"
colors = {
    'Indian Red':0xcd5c5c
  , 'Saddle Brown':0x8b4513
  , 'Yellow':0xffff00
  , 'Medium Sea Green':0x3cb371
  }
example1.dictKeys(colors)

print "\n\n"
print "------ modify a sequence in place ------"
ints = range(20)
ints.reverse()
print "original sequence: %s" % ints
example1.modifySeq(ints)
print "modified sequence: %s" % ints
