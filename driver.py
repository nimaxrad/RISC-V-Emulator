#!/usr//bin/python3
#
# driver.py - The driver tests the correctness
import subprocess
import re
import os
import sys
import argparse
import shutil
import json

running = {
}

grading = {

}
# Configure maxscores here
tests_json = """{
  "R": {
    "Part1": {
      "./riscv -d ./code/input/R/R.input > ./code/out/R/R.solution": 2,
      "python3 compare.py ./code/out/R/R.solution ./code/ref/R/R.solution": 10
    },
    "Part2": {
      "timeout 2 ./riscv -r -t ./code/input/R/R.input > ./code/out/R/R.trace": 0,
      "python3 compare.py ./code/out/R/R.trace ./code/ref/R/R.trace": 0
    }
  },
    "Ri": {
    "Part1": {
      "./riscv -d ./code/input/Ri/Ri.input > ./code/out/Ri/Ri.solution": 0,
      "python3 compare.py ./code/out/Ri/Ri.solution ./code/ref/Ri/Ri.solution": 0
    },
    "Part2": {
      "timeout 2 ./riscv -r -t -v ./code/input/Ri/Ri.input > ./code/out/Ri/Ri.trace": 2,
      "python3 compare.py ./code/out/Ri/Ri.trace ./code/ref/Ri/Ri.trace": 10
    }
  },
  "I": {
    "Part1": {
      "./riscv -d ./code/input/I/I.input > ./code/out/I/I.solution": 2,
      "python3 compare.py ./code/out/I/I.solution ./code/ref/I/I.solution": 10,
      "./riscv -d ./code/input/I/L.input > ./code/out/I/L.solution": 2,
      "python3 compare.py ./code/out/I/L.solution ./code/ref/I/L.solution": 10
    },
    "Part2": {
      "timeout 2 ./riscv -r -t ./code/input/I/I.input > ./code/out/I/I.trace": 2,
      "python3 compare.py ./code/out/I/I.trace ./code/ref/I/I.trace": 10,
      "timeout 2 ./riscv -r -t ./code/input/I/L.input > ./code/out/I/L.trace": 2,
      "python3 compare.py ./code/out/I/L.trace ./code/ref/I/L.trace": 10
    }
  },
  "S": {
    "Part1": {
      "./riscv -d ./code/input/S/S.input > ./code/out/S/S.solution": 2,
      "python3 compare.py ./code/out/S/S.solution ./code/ref/S/S.solution": 10
    },
    "Part2": {
      "timeout 2 ./riscv -r -t ./code/input/S/S.input > ./code/out/S/S.trace": 2,
      "python3 compare.py ./code/out/S/S.trace ./code/ref/S/S.trace": 10
    }
  },
  "SB": {
    "Part1": {
      "./riscv -d ./code/input/SB/SB.input > ./code/out/SB/SB.solution": 2,
      "python3 compare.py ./code/out/SB/SB.solution ./code/ref/SB/SB.solution": 10
    },
    "Part2": {
      "timeout 2 ./riscv -r -t ./code/input/SB/SB.input > ./code/out/SB/SB.trace": 2,
      "python3 compare.py ./code/out/SB/SB.trace ./code/ref/SB/SB.trace": 10
    }

  },
  "U": {
    "Part1": {
      "./riscv -d ./code/input/U/U.input > ./code/out/U/U.solution": 2,
      "python3 compare.py ./code/out/U/U.solution ./code/ref/U/U.solution": 10
    },
  "Part2": {
      "timeout 2 ./riscv -r -t ./code/input/U/U.input > ./code/out/U/U.trace": 2,
      "python3 compare.py ./code/out/U/U.trace ./code/ref/U/U.trace": 10
    }

  },
  "UJ": {
    "Part1": {
      "./riscv -d ./code/input/UJ/UJ.input > ./code/out/UJ/UJ.solution": 2,
      "python3 compare.py ./code/out/UJ/UJ.solution ./code/ref/UJ/UJ.solution": 10
    },
    "Part2": {
      "timeout 2 ./riscv -r -t ./code/input/UJ/UJ.input > ./code/out/UJ/UJ.trace": 2,
      "python3 compare.py ./code/out/UJ/UJ.trace ./code/ref/UJ/UJ.trace": 10
    }
  },
  "lswc": {
     "Part1": {
       "./riscv -d ./code/input/custom_lswc.input > ./code/out/custom_lswc.solution": 0,
       "python3 compare.py ./code/out/custom_lswc.solution ./code/ref/custom_lswc.solution": 30
     },
     "Part2": {
       "timeout 2 ./riscv -t -e -r -s ./code/input/lswc_data.input  -a 0x8,0x3000 ./code/input/custom_lswc.input > code/out/custom_lswc.trace": 0,
       "python3 part2_tester.py custom_lswc": 120
     }
   },
  "All": {
    "Part1": {
      "./riscv -d ./code/input/simple.input > ./code/out/simple.solution": 0,
      "python3 compare.py ./code/out/simple.solution ./code/ref/simple.solution": 30,
      "./riscv -d ./code/input/multiply.input > ./code/out/multiply.solution": 0,
      "python3 compare.py ./code/out/multiply.solution ./code/ref/multiply.solution": 30,
      "./riscv -d ./code/input/random.input > ./code/out/random.solution": 0,
      "python3 compare.py ./code/out/random.solution ./code/ref/random.solution": 30
    },
    "Part2": {
      "timeout 2 ./riscv -r -t -e ./code/input/simple.input > ./code/out/simple.trace": 0,
      "python3 part2_tester.py simple": 30,
      "timeout 2 ./riscv -r -t -e ./code/input/multiply.input > ./code/out/multiply.trace": 0,
      "python3 part2_tester.py multiply": 30,
      "timeout 2 ./riscv -r -t -e ./code/input/random.input > ./code/out/random.trace": 0,
      "python3 part2_tester.py random": 30
    }
  }
}
"""


  # "mac": {
  #   "Part1": {
  #     "./riscv -d ./code/input/custom_mac.input > ./code/out/custom_mac.solution": 0,
  #     "python3 compare.py ./code/out/custom_mac.solution ./code/ref/custom_mac.solution": 15
  #   },
  #   "Part2": {
  #     "timeout 2 ./riscv -r -t ./code/input/custom_mac.input > ./code/out/custom_mac.trace": 0,
  #     "python3 compare.py ./code/out/custom_mac.trace ./code/ref/custom_mac.trace": 15
  #   }
  # },
  # "acc": {
  #   "Part1": {
  #     "./riscv -d ./code/input/custom_acc.input > ./code/out/custom_acc.solution": 0,
  #     "python3 compare.py ./code/out/custom_acc.solution ./code/ref/custom_acc.solution": 15
  #   },
  #   "Part2": {
  #     "timeout 2 ./riscv -r -t ./code/input/custom_acc.input > ./code/out/custom_acc.trace": 0,
  #     "python3 compare.py ./code/out/custom_acc.trace ./code/ref/custom_acc.trace": 15
  #   }
  # },
  # "gep": {
  #   "Part1": {
  #     "./riscv -d ./code/input/custom_gep.input > ./code/out/custom_gep.solution": 0,
  #     "python3 compare.py ./code/out/custom_gep.solution ./code/ref/custom_gep.solution": 15
  #   },
  #   "Part2": {
  #     "timeout 2 ./riscv -r -t ./code/input/custom_gep.input > ./code/out/custom_gep.trace": 0,
  #     "python3 compare.py ./code/out/custom_gep.trace ./code/ref/custom_gep.trace": 15
  #   }
  # },



Final = {}
#
# main - Main function
#


def main():
    # Parse the command line arguments
    Error = ""
    Success = ""
    PassOrFail = 0
    parser = argparse.ArgumentParser()
    parser.add_argument("-A", action="store_true", dest="autograde",
                        help="emit autoresult string for Autolab")

    parser.add_argument("-D", dest="output",
                        help="output directory", required=True)

    opts = parser.parse_args()
    autograde = opts.autograde
    output_folder = opts.output
    if os.path.exists(output_folder):
        shutil.rmtree(output_folder)

    test_dict = json.loads(tests_json)

    # Check the correctness and performance of the transpose function
    # 32x32 transpose
    for r in test_dict.keys():
        if not os.path.exists(output_folder+"/"+r):
            os.makedirs(output_folder + "/" + r)
        # Reset points
        for parts in ("Part1", "Part2"):
            total = 0
            points = 0
            for tests in test_dict[r][parts].keys():
                p = subprocess.Popen(tests,
                                     shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                stdout_data, stderr_data = p.communicate()
                total = total + test_dict[r][parts][tests]
                if(p.returncode != 0):
                    Error += "### " + "*"*5+tests+"*"*5
                    Error += "\n ```" + stdout_data.decode()
                    Error += "\n```\n"
                    PassOrFail = 1
                else:
                    points += test_dict[r][parts][tests]
                    Success += "### " + "*"*5+tests+"*"*5
                    Success += "\n ```" + stdout_data.decode() + "\n```\n"

            if points < total/2:
                Final[(r + parts).lower()] = {"mark": points,
                                              "comment": "Program did not run successfully. It either did not build or exited with error code 0"}
            elif points < total:
                Final[(r + parts).lower()] = {"mark": points,
                                              "comment": "Program ran, but output did not match. see log file"}
            else:
                Final[(r + parts).lower()] = {"mark": points,
                                              "comment": "Program ran and output matched."}
        # # Reset Points
        # total = 0
        # points = 0
        # for tests in test_dict[r]["Part2"].keys():
        #     p = subprocess.Popen(tests,
        #                          shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #     stdout_data, stderr_data = p.communicate()
        #     total = total + test_dict[r]["Part2"][tests]
        #     if(p.returncode != 0):
        #         Error += "*"*20+tests+"*"*20
        #         Error += "\n" + stdout_data.decode()
        #     else:
        #         points += test_dict[r]["Part2"][tests]
        #         Success += "*"*20+tests+"*"*20
        #         Success += "\n" + stdout_data.decode()

        # if points < total/2:
        #     Final[r + "-P2"] = {"mark": points,
        #                         "comment": "Program did not run successfully. It either did not build or exited with error code 0"}
        # elif points < total:
        #     Final[r + "-P2"] = {"mark": points,
        #                         "comment": "Program ran, but output did not match. see log file"}
        # else:
        #     Final[r+"-P2"] = {"mark": points,
        #
        #              "comment": "Program ran and output matched."}
    githubprefix = os.path.basename(os.getcwd())
    Final["userid"] = "GithubID:" + githubprefix
    j = json.dumps(Final, indent=2)

    with open(githubprefix + "_Grade"+".json", "w+") as text_file:
        text_file.write(j)

    with open("LOG.md", "w+") as text_file:
        text_file.write("## " + '*'*20 + 'FAILED' + '*'*20 + '\n' + Error)
        text_file.write("\n" + "*" * 40)
        text_file.write("\n## " + '*'*20 + 'SUCCESS' + '*'*20 + '\n' + Success)

    sys.exit(PassOrFail)


# execute main only if called as a script
if __name__ == "__main__":
    main()
