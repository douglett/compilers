# compute sum or product

print "input a number"
input n
print "[add] or [mul]?"
input type

# add
eq type "add"
if pop then
	print "add selected"
	let i "1"
	let ans "0"
	do
		add ans i
		add i "1"
		lt i n
		if pop then
			continue
		end
		print "add 1 to" n "==" ans
	end
	break
end

# multiply
eq type "mul"
if pop then
	print "mul selected"
	let i "1"
	let ans "1"
	do
		mul ans i
		add i "1"
		lt i n
		if pop then
			continue
		end
		print "mul 1 to" n "==" ans
	end
	break
end

# unknown
print "error: unknown input" type