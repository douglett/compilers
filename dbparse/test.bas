dim a = 10
dim b = 20
dim c
dim i = 0
dim foo[20]
dim bar[] = { 1, 2, 3 }
dim foobar[] = "some text"

; stuff
if (a == (b / 2))
	print a b foobar$
elif (a == c)
	print "unknown"
end

i = 10
while (i > 0)
	print "test" i
end

:blah
i = 10
if (i == 9)
	goto blah
end