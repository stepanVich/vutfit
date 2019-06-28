#!/usr/bin/env python3

class Polynomial:
	"""Class for addition, exponentiation and printing math polynoms."""
	
	polynom_list=[0]
	"""list: Include coefficient of the particular polynom members."""

	def __init__(self, *args, **kwargs):
		"""Initialize the polynom and parse all arguments.

		Args:
			*args: Variable length of coefficient.
			**kwargs: Dictionary, where x[A] contains coefficient of A member.
		
		Examples:
			>>> print(Polynomial(1,2,3))
			3x^2 + 2x + 1
			
			>>> print(Polynomial([1,2,3]))
			3x^2 + 2x + 1
		
			>> print(Polynomial(x1=2,x0=1,x2=3))
			3x^2 + 2x + 1
		"""
		if args:
			# If first argument is list, then read coefficient from this list.
			if isinstance(args[0], list):
				self.polynom_list=list(args[0])
			else:
				self.polynom_list=list(args)
		
		elif kwargs:
			for i in kwargs:
				# Check if key start with x
				number = ""
				if i[:1] == "x" and len(i[1:]) > 0:
					number = i[1:]
				# Check if key end with number
				if not number.isdigit(): continue
				# Add coefficient to polynom.
				number = int(number)
				while len(self.polynom_list) <= number: self.polynom_list.append(0)
				self.polynom_list[number] = kwargs[i]

		# Remove end zeros from polynom.
		end_index = len(self.polynom_list) - 1
		while end_index > 0 and self.polynom_list[end_index] == 0:
			self.polynom_list.pop()
			end_index -= 1
			if end_index < 0: break

		return None

	def __add__(self, other):
		"""Return self+other."""
		# Select bigger list.
		if len(self.polynom_list) > len(other.polynom_list):
			big_list = self.polynom_list[:]
			small_list = other.polynom_list[:]
		else:
			big_list = other.polynom_list[:]
			small_list = self.polynom_list[:]

		# Iterate over bigger list.
		for i, val in enumerate(big_list):
			# Add value to coefficient.
			if len(small_list) > i:
				big_list[i] = val + small_list[i]
			else:
				big_list[i] = val

		return Polynomial(big_list)

	
	def __pow__(self, other):
		"""Return self**other."""

		pow_list = self.polynom_list[:]
		# Help variable
		store_list = []
		if str(other).isdigit():
			loop_count = int(other)
			for i in range(1, loop_count):
				# AxB loop, where every coefficient is paired with another.
				for a, val_a in enumerate(self.polynom_list):
					for b, val_b in enumerate(pow_list):
						index = a + b
						value = val_a*val_b
						while len(store_list) <= index: store_list.append(0)
						store_list[index] += value
				# Restore variables, before next loop
				pow_list = store_list
				store_list = []
		else:
			print("Pow method only accept positive natural numbers.")
			return Polynomial()
		return Polynomial(pow_list)

	def derivative(self):
		"""Return derivated polynom."""
		derivate_list = []
		for i, val in enumerate(self.polynom_list):
			# In derivation, didn`t work with constants.
			if i == 0: continue
			derivate_list.append(i*val)

		return Polynomial(derivate_list)


	def _at_value_calc(self, x=0):
		""" Private method, for less code."""
		value = 0
		for i, val in enumerate(self.polynom_list):
			value = value + x**i*val
		return value

	def at_value(self, *args):
		"""Return value of polynom in the specified point.

		Args:
		    *args[0] (int): The first point. Default is zero.
		    *args[1] (Optional[int]): The second point. Default to None.

		Returns:
		    int: If second point is defined, return difference between second and first point.
			 Otherwise return value of first point.
		"""		
		if len(args) == 1:
			return self._at_value_calc(args[0])
		elif len(args) == 2:
			return self._at_value_calc(args[1]) - self._at_value_calc(args[0])

	def __str__(self):
		"""Return string for print() method."""
		# Define constant values.
		string = ""
		plus_sign = " + "
		minus_sign = " - "
		# Iterate over polynom coefficient
		for i, val in enumerate(self.polynom_list):
			sign = plus_sign

			# If coefficient == 0, don`t print entire x member.
			if val == 0:
				continue

			if val < 0:
				val = -val;
				sign = minus_sign

			
			if i == len(self.polynom_list) - 1:
				# For first member, clean sign, if is positive.			
				if sign == plus_sign:
					sign = ""
				# For first member, strip space before sign, if is negative
				if sign == minus_sign:
					sign = "- "						 

			val = str(val)
			# If coefficient == 1, don`t print them.
			if val == "1" and i != 0:
				val = ""

			# Adding member to printed string.
			if i == 0:
				string = sign + val
			elif i == 1:
				string = sign + val + "x" + string
			else:
				string = sign + val + "x^" + str(i) + string

		return string
