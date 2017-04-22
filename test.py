import sys

if len(sys.argv) != 2:
	print("Usage: python3 " + sys.argv[0] + " [freq_file]")
	exit(1)

with open(sys.argv[1]) as f:
	correct_string = f.readline()
	correct_binary = ""

	received_string = ""
	received_binary = ""

	for line in f:
		freqs = line.strip().split(" ")
		freqs = list(map(int, freqs))

		tmp = ""
		for f in range(195, 203):
			if f in freqs:
				tmp += "1"
			else:
				tmp += "0"

		received_binary += tmp[::-1] + " "
		received_string += chr(int(tmp[::-1], 2)) + "        "

		tmp = ""
		for f in range(203, 211):
			if f in freqs:
				tmp += "1"
			else:
				tmp += "0"

		received_binary += tmp[::-1] + " "
		received_string += chr(int(tmp[::-1], 2)) + "        "

	for c in correct_string.strip():
		correct_binary += bin(ord(c))[2:].zfill(8) + " "
		sys.stdout.write(c + "        ")

	sys.stdout.write("\n")

print(correct_binary)
print(received_binary)
print(received_string)

