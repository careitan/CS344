#!/usr/bin/env python3
#
# OSU Fall 2017 CS344 - Program Py
# (Craig) Allan Reitan
# 10-16-2017
#
########
#
# Importing stuff
import random
# Start with the random number generator seed.
random.seed(None)

# Add a function to be called that will create 10 random numbers in a rand_list variable.
# Then attach that rand_list to a bytearray() object and use the str.encode() to get it to convert to letters.
# Append the '\n' to the end of the bytearray.
# Print() the ByteArray() out, then pass that into a file object.
def randomChars(file_name):
	rand_list = random.sample(range(97, 123, 1), 10) # The range should be the ASCII numbers for lowercase letters a - z.
	output = bytearray(rand_list)
	output.extend(b'\n')
	print(bytearray(rand_list).decode("utf-8"))
	f = open(file_name, 'w')
	f.write(output.decode("utf-8"))
	# Close the open file
	f.close()

files = ['Larry', 'Moe', 'Curley']
# Make three function calls to each of the random generator and file creation.
for i in files:
	randomChars(i)

# Make two variables and just enter random generated numbers.
# Then print them out with the product between them.
a=random.randrange(1, 43, 1)
b=random.randrange(1, 43, 1)
print(a)
print(b)
print(a*b)

# Exit