import difflib
import os
import subprocess
import sys
import unittest

COMPILER_NAME = "./cplus"
PROGRAM_NAME = "./a.out"

SOURCE_EXT = ".cp" # file extension to distinguish test source files from...
ANSWER_EXT = ".ans" # expected results

class ExampleTest(unittest.TestCase):
    cases = []

    def setUp(self):
        self.did_run = { test : False for test in self.cases }
        # keep outputs since output file is not persistent
        self.outputs = dict()

    def tearDown(self):
        for test_name, run_status in self.did_run.items():
            if run_status:
                # add failed test description into report file
                with open("report.txt", "a") as file:
                    differents = difflib.ndiff(self.outputs[test_name][0], self.outputs[test_name][1])
                    file.write(10 * ">" + f" FILE: {test_name} " + 10 * "<" + "\n")
                    file.write("".join(differents))

    def test_example(self):
        for example in self.cases:
            with self.subTest():
                if sys.platform.startswith("win"): # Windows or Linux
                    compiler_name = "./cplus.exe"
                    program_name = "./program.exe"
                else:
                    compiler_name = COMPILER_NAME
                    program_name = PROGRAM_NAME                
                try:
                    # run Cplus compiler
                    subprocess.check_call(f"{compiler_name} ./{example + SOURCE_EXT}", stdin=None, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, shell=False)

                    # run generated program and write its output to "out.txt"
                    with open("out.txt", "w") as actual:
                        subprocess.call(program_name, stdin=None, stdout=actual, stderr=None, shell=False)
                    
                    self.did_run[example] = True

                    # read program output and compare results, set "Passed" flag on success
                    with open(f"{example}" + ANSWER_EXT) as expected:
                        with open("out.txt", "r") as actual:
                            expected_text = expected.readlines()
                            actual_text = actual.readlines()

                            # store data to examine later
                            self.outputs[example] = [expected_text, actual_text]

                            self.assertEqual("\n".join(expected_text), "\n".join(actual_text))
                            print(f"Test \"{example + SOURCE_EXT}\" succeded.")
                except AssertionError as e:
                    print(f"Test \"{example + SOURCE_EXT}\" failed. Outputs are different. See more in report.txt")
                except Exception as e:
                    print(f"Test \"{example + SOURCE_EXT}\" failed. Reason: {e}")

if __name__ == "__main__":
    # grab directory with test cases
    assert len(sys.argv) == 2, "You must provide a path to test directory. Example: \"python test.py ./examples/\"."
    test_dir = sys.argv[1]

    assert os.path.isfile(COMPILER_NAME) or os.path.isfile(COMPILER_NAME + ".exe"), "Cplus compiler must be in the same folder as tests.py"

    # clear report file
    open("report.txt", "w").close()
    
    # traverse test directry and discover test cases
    for root, dirs, files in os.walk(test_dir):
        # only "*.cp" files are test case sources
        for case in filter(lambda name : name.endswith(SOURCE_EXT), files):
            test_name = os.path.splitext(root + case)[0]
            # check that corresponding expected "*.ans" file exists
            if os.path.isfile(test_name + ANSWER_EXT):
                ExampleTest.cases.append(test_name)
            else:
                print(f"TEST WARNING: Test \"{test_name + SOURCE_EXT}\" does not have corrensponding \"{test_name + ANSWER_EXT}\". Test will be skipped.")
    # run test suite
    unittest.main(argv=["first-arg-is-ignored"])