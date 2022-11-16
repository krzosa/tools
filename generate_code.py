import subprocess
import sys
import os

python_exe = "python3"
files = ["mui.c"]

for file_to_modify in files:
    fd = open(file_to_modify, "r+")
    f = fd.read()
    END = "/*END*/"
    START = "/*#"
    STOP = "*/"

    program_counter = 0
    valid = True
    index = 0
    while True:
        begin = f.find(START, index)
        if begin == -1:
            break
        index = begin
        begin += 3

        end = f.find(STOP, index)
        if end == -1:
            valid = False
            print(f"One of the registered comments inside {file_to_modify} doesn't have an end")
            break

        gen_end = f.find(END, end)
        next_block = f.find(START, end)
        if next_block != -1 and gen_end > next_block:
            gen_end = -1

        before = f[:begin]
        program = f[begin:end]
        if gen_end == -1:
            after = f[end+2:]
        else:
            after = f[gen_end+len(END):]

        temp_filename = "_metaprogram" + str(program_counter) + ".py"
        if os.path.isfile(temp_filename):
            print(temp_filename + " exists, dont want to risk overwriting something important")
            valid = False
            break
        program_counter += 1
        with open(temp_filename, "w") as meta_file:
            meta_file.write(program)

        result = subprocess.run([python_exe, temp_filename], stdout=subprocess.PIPE)
        program_result = result.stdout.decode('utf-8').replace('\r\n', '\n') + END
        f = before + program + "*/\n" + program_result + after
        os.remove(temp_filename)

        index = end

    if valid:
        fd.seek(0)
        fd.write(f)
        fd.truncate()
    fd.close()
