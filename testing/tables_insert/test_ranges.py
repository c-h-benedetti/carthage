import random

#            0        1          2         3         4         5
tables = [(5, 10), (15, 20), (25, 30), (35, 40), (45, 50), (55, 55)]


def find_table(id):

	size = len(tables)
	first = 0
	last = size - 1
	mid = 0

	while (first <= last):
		mid = int((first + last) / 2)
		if (id < tables[mid][0]):
			last = mid - 1
		else:
			first = mid + 1
		
	target = last

	if (target < 0):
		target = 0
	elif (target >= size):
		target = size - 1

	return {'target': target}
		
	
for i in range(0, 100):
	print(f"ID: {i} -> {find_table(i)}")

"""

			=== INVARIANTS ===

- If an ID is included in a range, the (target) will point to the table that should contain it.

- If an ID is not is any range, the target value provided is always the one of the above interval. 
  So target is in [0, size]. In this case, the key can be inserted at the end of the previous table
  or at the begining of the pointed table.

- The variable (in_range) enables to quickly know if the value is contained within an already known interval

- With the help of the first serie of tests, this algorithm should not create values below 0, hence the possibility
  to run it using only unsigned integers types.

!!! From the previous assumptions, you can deduce that (target) can be 0 either if the value is out of range
    and lower than the current lowest known value, or if it is suposed to be inserted in the middle of index 0.
    It is an example of (in_range)'s usage.
    Of the same way, (target) can contain a value which is out of bounds (== size of vector) and it indicates
    that the key can be inserted at the end of the last table, or in a new table.

"""