default:
	gcc -g -o ringmaster main.c

grade:
	python3 test/grader.py ./ringmaster test-cases