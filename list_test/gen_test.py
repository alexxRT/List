import os
import random
import sys
from pathlib import Path
test_folder = "list_test/" 

init = "ListInit (&list, elem_num);"
destroy = "ListDestroy (&list);"

insert_functions = ["ListInsertHead(&list, ", "ListInsert(&list, ", "ListInsertIndex(&list, "]

delete = "ListDelete(&list, "

resize = "ListResize(my_list* list);"
linirize = "MakeListGreatAgain (my_list* list);"

def start_main(file):
    if file == 0:
        print ("BAD FILE")
        return 0
    
    print("#include <stdio.h>",      file = file)
    print("#include <list.h>",       file = file)
    print("#include <list_debug.h>", file = file)

    print("\n\n\n", file = file)

    print ("int main() {", file=file)
    print ("InitLogFile(\"log.txt\");", file=file)
    

def init_list(file, num_elems):
    file.seek(os.SEEK_END)

    print("\tmy_list list = {};", file=file)
    print("\tsize_t elem_num = " + str(num_elems) + ";", file = file)
    print ("\t"+init, file=file)
    print ("\tLIST_ERR_CODE err_code = LIST_ERR_CODE::SUCCESS;", file = file)


def destroy_list(file):
    file.seek(os.SEEK_END)

    print("\t"+destroy, file=file)

def end_main(file):
    file.seek(os.SEEK_END)

    print("\tDestroyLogFile();", file = file)
    print("\tprintf(\"\033[32m\"\"TEST SUCCEEDED\\n\"\"\033[0m\");", file = file)
    print("\treturn 0;", file = file)
    print ("}", file = file)

def test_failed(file):
    print("\tif(err_code != LIST_ERR_CODE::SUCCESS){", file = file)
    print("\t\tPrintErr(&list, err_code, __LINE__, __func__);", file = file)
    print("\t\tDestroyLogFile();", file=file)
    print("\t\tprintf(\"\033[31m\"\"TEST FAILED\\n\"\"\033[0m\");\n", file = file)
    print("\t\treturn 777;}\n", file = file)

def list_linirize(file):
    print("\terr_code = MakeListGreatAgain(&list);", file = file)
    test_failed(file)

def alive_stat(file):
    print("\tprintf(\"alive test file line %d\\n\", __LINE__);", file=file)
    

def build_insert(file, capacity, size, n_i):
    file.seek(os.SEEK_END)

    if (n_i < 3):
        for i in range (0, n_i):
            print("\terr_code =", insert_functions[0] + str(i) + ");", file = file)
            test_failed(file)
        return 
    
    part = n_i // 3

    for i in range (0, part):
        print("\terr_code =", insert_functions[0] + str(i) + ");", file = file)
        test_failed(file)


    for i in range (0, part):
        pos = 0
        if (size <= 1):
            pos = size
        else:
            pos = random.randrange(1, size, 1)

        print("\terr_code =", insert_functions[1] + str(pos) + "," + str(i) + ");", file = file)
        test_failed(file)

    for i in range (0, n_i - 2*part):
        indx = random.randrange(1, capacity, 1)
        print("\terr_code =", insert_functions[2] + str(indx) + "," + str(i) + ");", file = file)
        test_failed(file)

def build_delete(file, size, n_d):
    file.seek(os.SEEK_END)

    for i in range (0, n_d):
        pos = 0
        if (size <= 1):
            pos = size
        else:
            pos = random.randrange(1, size, 1)
            size -= 1

        print("\terr_code =", delete + str(pos) + ");", file = file)
        test_failed(file)


def build_test(file, start_capacity, num_insert, num_delete):
    insert_count = 0
    delete_count = 0
    capacity = start_capacity

    print("\n//TEST BODY STARTED\n", file = file)

    while (num_insert != 0 or num_delete != 0):
        # inserts to generate
        if (num_insert > 0):
            i = 0
            if (num_insert == 1):
                i = 1
            else:
                i = random.randrange(1, num_insert, 1)
            
            build_insert(file, capacity, insert_count - delete_count, i)
            num_insert -= i
            insert_count += i

            #correct capcity due to list resize
            while (insert_count - delete_count >= capacity):
                capacity *= 2
        
        if (num_delete > 0):
            # deletes to generate
            d = 0
            if (i == 1):
                d = 1
            else:
                d = min(random.randrange(1, i, 1), num_delete)
            
            build_delete(file, insert_count - delete_count, d)
            num_delete -= d
            delete_count += d

            #correct list capacity due to resize
            while (insert_count - delete_count < capacity // 4 and capacity // 2 >= start_capacity):
                capacity //= 2

        #DOUBLE CHECK
        if (insert_count < delete_count):
            print("BAD TEST GENERATED, ABORTED", file.name)
            break

    print("\n//TEST BODY FINISHED\n", file = file)


def add_test_to_build(file, test_path):
    file.seek(os.SEEK_END)

    test_name = Path(test_path).stem

    print(f"add_executable({test_name} {test_path})", file = file)
    print(f"target_link_libraries({test_name} list)", file = file)
    
    with open(test_folder + "run_test.txt", mode = "a") as f:
        f.seek(os.SEEK_END)
        print(f"./{test_name}", file = f)
        
    return

def generate_test(file_name: str, init_capacity: int, num_insert: int, num_delete: int):
    file = open(test_folder + file_name, mode="a")

    start_main(file)
    init_list(file, init_capacity)
    build_test(file, init_capacity, num_insert, num_delete)
    destroy_list(file)
    end_main(file)

    file.close()

    build_file = open("CMakeLists.txt", mode="a")
    add_test_to_build(build_file, test_folder + file_name)

    build_file.close()
    

if __name__ == "__main__":
    if (len(sys.argv) < 5):
        print("LACK OF ARGUMENTS")

    else:
        generate_test(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]))