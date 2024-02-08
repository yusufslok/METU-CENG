for _ in range(160000):
	a = input()
	y = a.find("?")
	checkDigit = a[5]
	if checkDigit == "X":
		checkDigit = 10
	elif checkDigit != "?":
		checkDigit = int(checkDigit)
	if y == 0:
		print(str(((((-(int(a[1]) * 3 + int(a[2]) * 5 + int(a[3]) * 7) % 11)) + checkDigit + 10) * 6 + 6) % 11) + a[1:6])
	elif y == 1:
		print((a[0] + str(((((-(int(a[0]) * 2 + int(a[2]) * 5 + int(a[3]) * 7) % 11)) + checkDigit + 10) * 4 + 4) % 11)) + a[2:6])
	elif y == 2:
		print(a[0:2] + str(((((-(int(a[1]) * 3 + int(a[0]) * 2 + int(a[3]) * 7) % 11)) + checkDigit + 10) * 9 + 9) % 11) + a[3:])
	elif y == 3:
		print(a[:3] + str(((((-(int(a[1]) * 3 + int(a[0]) * 2 + int(a[2]) * 5) % 11)) + checkDigit + 10) * 8 + 8) % 11) + a[4:])
	elif y == 5:
		if (int(a[0]) * 2 + int(a[1])* 3 + int(a[2]) * 5 + int(a[3]) * 7) % 11 == 10:
			print(a[:5] + "X")
		else:
			print(a[:5] + str((int(a[0]) * 2 + int(a[1])* 3 + int(a[2]) * 5 + int(a[3]) * 7) % 11))
	elif y == -1:
		if (int(a[0]) * 2 + int(a[1]) * 3 + int(a[2]) * 5 + int(a[3]) * 7) % 11 == checkDigit:
			print("VALID")
		else:
			print("INVALID")
