; multiple dim types
dim a = 10
dim b = 20
dim c
dim i = 0
dim foo[20]
dim bar[] = { 1, 2, 3 }
dim foobar[] = "some text"

; if block
if (a == (b / 2))
	print a b foobar$
elif (a == c)
	print "unknown"
end

; while block
i = 10
while (i > 0)
	print "test" i
end

; goto
:blah
i = 10
if (i == 9)
	goto blah
end

; string manipulation
while (1)
	dim text1[] = "foo"
	dim text2[] = "bar"
	concat text1 text2
	concat text1 "blah"
	print "foobar " text1$
	break
end