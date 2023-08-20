# ccalc
A C version of JCalc using MPFR, GMP and Readline libraries. The calculator converts the entered calculation into Reverse Polish Notation (RPN) using the 'Shunting Yard' algorithm before evaluating the result.

**Copyright (C) 2023  Guy Wilson**
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions.

## Operators supported:
	+, -, *, /, % (Modulo)
	& (AND), | (OR), ~ (XOR)
	<< (left shift), >> (right shift)
	^ (power, e.g. x to the power of y)
	: (root, e.g. x : y - the yth root of x)
    
	Nesting is achieved with braces ()

## Functions supported:
	sin(x)	return the sine of the angle x degrees
	cos(x)	return the cosine of the angle x degrees
	tan(x)	return the tangent of the angle x degrees
	asin(x)	return the angle in degrees of arcsine(x)
	acos(x)	return the angle in degrees of arccosine(x)
	atan(x)	return the angle in degrees of arctangent(x)
	sinh(x)	return the hyperbolic sine of the angle x radians
	cosh(x)	return the hyperbolic cosine of the angle x radians
	tanh(x)	return the hyperbolic tangent of the angle x radians
	asinh(x) return the inverse hyperbolic sine of angle x in radians
	acosh(x) return the inverse hyperbolic cosine of angle x in radians
	atanh(x) return the inverse hyperbolic tangent of angle x in radians
	sqrt(x)	return the square root of x
	log(x)	return the log of x
	ln(x)	return the natural log of x
	fact(x)	return the factorial of x
	mem(n)	the value in memory location n, where n is 0 - 9

## Constants supported:
	pi	the ratio pi
	eu	Eulers constant
	g	The gravitational constant G
	c	the speed of light in a vacuum

## Commands supported:
	memstn	Store the last result in memory location n (0 - 9)
	dec	Switch to decimal mode
	hex	Switch to hexadecimal mode
	bin	Switch to binary mode
	oct	Switch to octal mode
	deg	Switch to degrees mode for trigometric functions
	rad	Switch to radians mode for trigometric functions
	setpn	Set the precision to n
	help	This help text
	test	Run a self test of the calculator
	exit	Exit the calculator
