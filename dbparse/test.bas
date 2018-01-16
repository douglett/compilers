; multiple dim types
dim a = 10
dim b = 20
dim c
dim i = 0
dim foo[20]
dim bar[] = { 1, 2, 3 }
dim foobar[] = "some text"
dim foobaz[256] = "some text"

; if block
if (a + 1 == (b / 2))
	print a b foobar$
elif (a == c)
	print "a is c"
else
	print "unknown"
end

; while block
i = 10
while (i > 0)
	print "test" i
	if (i == 2)
		print "blah number 2"
	end
end

; goto
blah:
i = 10
if (i == 9)
	goto blah
	; goto balls
end

; string manipulation
while (1)
	dim text1[] = "foo"
	dim text2[] = "bar"
	; concat text1 text2
	; concat text1 "blah"
	print "foobar " text1$
	break
end