COMP_PATH="../src/IFJ24comp"
IC_PATH="./ic24int"
test_succ=0
test_fail=0
valgrind_output="valgrind_output.txt"
use_valgrind=false
run_all=false
run_noic=false
run_ic=false

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function log_success() {
  echo -e "${GREEN}$1${NC}"
}

function log_error() {
  echo -e "${RED}$1${NC}"
}

function log_info() {
  echo -e "${YELLOW}$1${NC}"
}

function run_make() {
  echo "Trying build IFJ24 executable"

  echo "Running make"
  cd ../src && make >/dev/null 2>&1
  if [ $? -eq 0 ]; then
    log_success "✅Build successful"
  else
    log_error "❌ Build failed"
    exit -1
  fi
  cd ../tests/
}

function run_tests() {
  if [[ "$run_all" == true || "$run_ic" == true ]]; then
    log_info "
    ------------INTERPRET TEST------------
    "
    for zig_file in examples/example*.zig; do
      base_name=${zig_file%.zig} # Убираем расширение .zig

      for input_file in "${base_name}"_*.in; do
        num=${input_file##*_}
        num=${num%.in}

        output_file="${base_name}_${num}.out"

        if [[ -f "$output_file" ]]; then
          test_with_ic "$zig_file" "$input_file" "$output_file"
        fi
      done
    done
  fi

  if [[ "$run_all" == true || "$run_noic" == true ]]; then
    log_info "
    ------------GENERAL TEST WITH NO ERRORS------------
    "
    test "examples/example01.zig" 0
    test "examples/example02.zig" 0
    test "examples/example03.zig" 0
    test "examples/example04.zig" 0
    test "examples/example05.zig" 0
    test "examples/example06.zig" 0
    test "examples/example07.zig" 0
    test "examples/example08.zig" 0
    test "examples/example09.zig" 0
    test "examples/example10.zig" 0
    test "examples/example11.zig" 0
    test "examples/example12.zig" 0
    test "examples/example13.zig" 0
    test "examples/example14.zig" 0
    test "examples/example15.zig" 0
    test "examples/example16.zig" 0
    test "examples/example17.zig" 0
    test "examples/example18.zig" 0
    test "examples/example19.zig" 0
    test "examples/example20.zig" 0
    test "examples/example21.zig" 0
    test "examples/example43.zig" 0

    log_info "
    ------------TESTS FOR BASE ERRORS FROM PDF------------
    "
    # Undefined function (Error 3)
    test "./base/base_undefined_function.zig" 3
    # Missing main function (Error 3)
    test "./base/base_missing_main.zig" 3
    # Function overloading (Error 5)
    test "./base/base_overloading.zig" 5
    # Nested function definition (Error 2)
    test "./base/base_nested.zig" 2
    # Return value type mismatch (Error 4 or 6)
    test "./base/base_return_missing.zig" 6
    test "./base/base_return_mismatch.zig" 4
    test "./base/base_return_mismatch_void.zig" 6
    # Argument number and type mismatch (Error 4)
    test "./base/base_arguments_number.zig" 4
    test "./base/base_arguments_mismatch.zig" 4
    # Ignoring return value (Error 4)
    test "./base/base_return_ignore.zig" 4
    # Variable scoping and definition errors
    test "./base/base_var_hoisting.zig" 3
    test "./base/base_var_shadowing.zig" 5
    test "./base/base_var_unused.zig" 9
    # Type inference and compatibility errors
    test "./base/base_var_infer.zig" 8
    test "./base/base_var_type.zig" 8
    test "./base/base_implicit_conversion.zig" 7

    log_info "
    ------------EXPRESSION PARSER TESTS------------
    "
    test "./expression/expr_sem_1.zig" 0 # ?
    test "./expression/expr_sem_2.zig" 0 # ?
    test "./expression/expr_type_1.zig" 7
    test "./expression/expr_type_2.zig" 7
    test "./expression/expr_type_3.zig" 7
    test "./expression/expr_type_4.zig" 0 # ?
    test "./expression/expr_type_5.zig" 7
    test "./expression/expr_syn_1.zig" 2
    test "./expression/expr_syn_2.zig" 2
    test "./expression/expr_syn_3.zig" 2
    test "./expression/expr_syn_4.zig" 2
    test "./expression/expr_syn_5.zig" 2
    test "./expression/expr_syn_6.zig" 2
    test "./expression/expr_syn_7.zig" 2

    log_info "
    ------------TESTS FOR LEXICAL ERRORS------------
    "
    for test_file in "./lexerrors"/*.zig; do
      if [ -f "$test_file" ]; then
        test "$test_file" 1 # Assuming all lexical error tests should return code 1
      fi
    done

    log_info "
    ------------TESTS FOR SYNTAX ERRORS------------
    "
    for test_file in "./synerrors"/*.zig; do
      if [ -f "$test_file" ]; then
        test "$test_file" 2 # Syntax errors return code 2
      fi
    done

    log_info "
    ------------TESTS FOR SEMANTIC ERRORS------------
    "

    for test_file in "./semerrors"/*.zig; do
      if [ -f "$test_file" ]; then
        # For semantic errors, we need to determine the correct return code
        # based on the file name
        if [[ $test_file == *"undef"* ]]; then
          test "$test_file" 3 # Undefined function/variable

        elif [[ $test_file == *"par_type"* ]]; then
          test "$test_file" 4 # Type compatibility error

        elif [[ $test_file == *"redef"* ]]; then
          test "$test_file" 5 # Parameter count/type mismatch

        elif [[ $test_file == *"ret"* ]]; then
          test "$test_file" 6 # Other semantic errors

        elif [[ $test_file == *"wrg_type"* ]]; then
          test "$test_file" 7 # Assignment compatibility error

        elif [[ $test_file == *"ded_type"* ]]; then
          test "$test_file" 8 # Assignment compatibility error

        elif [[ $test_file == *"unused"* ]]; then
          test "$test_file" 9 # Assignment compatibility error

        else
          test "$test_file" 10 # Other semantic error code
        fi
      fi
    done
  fi
  log_info "
  ------------TESTS FOR FUNEXP------------
  "
  test_with_ic "./funexp/funexp01.zig" "./funexp/funexp01_1.in" "./funexp/funexp01_1.out"
  test_with_ic "./funexp/funexp02.zig" "./funexp/funexp02_1.in" "./funexp/funexp02_1.out"
  test "./funexp/funexp03.zig" 7
  test "./funexp/funexp04.zig" 2
  test_with_ic "./funexp/funexp05.zig" "./funexp/funexp05_1.in" "./funexp/funexp05_1.out"
  test_with_ic "./funexp/funexp06.zig" "./funexp/funexp06_1.in" "./funexp/funexp06_1.out"
  test_with_ic "./funexp/funexp07.zig" "./funexp/funexp07_1.in" "./funexp/funexp07_1.out"
  test "./funexp/funexp08.zig" 4
  test "./funexp/funexp09.zig" 2
  test "./funexp/funexp10.zig" 2
  test_with_ic "./funexp/funexp11.zig" "./funexp/funexp11_1.in" "./funexp/funexp11_1.out"
  test "./funexp/funexp12.zig" 4
  test "./funexp/funexp13.zig" 4
  test "./funexp/funexp14.zig" 4
  test "./funexp/funexp15.zig" 2
  test "./funexp/funexp16.zig" 2

  log_info "
  ------------COMPLEX TESTS------------
  "
  test_with_ic "./complex/complex_01.zig" "./complex/complex_01.in" "./complex/complex_01.out"

  echo "
  ✅ Successful tests $test_succ
  ❌ Failed tests $test_fail"
}

#$1 -> zig
#$2 -> input to ic
#$3 -> expected output from ic
function test_with_ic() {
  input=$1
  inputic=$2
  outputic=$3

  $COMP_PATH <$input >prg.code
  $IC_PATH prg.code <$inputic >prg.out

  if diff -q prg.out $outputic >/dev/null; then
    log_success "
      ✅ Test for $inputic finished successfully"
    ((test_succ += 1))
  else
    log_error "
      ❌ Test failed"
    diff prg.out $outputic
    ((test_fail += 1))
  fi
}

function test() {
  input=$1
  code=$2

  if $use_valgrind; then
    valgrind --leak-check=full --error-exitcode=1 $COMP_PATH <$input >/dev/null 2>$valgrind_output
  else
    $COMP_PATH <$input >/dev/null 2>&1
  fi

  return_code=$?

  if [ $return_code -eq $code ]; then

    if $use_valgrind; then
      if grep -q "ERROR SUMMARY: 0 errors from 0 contexts" $valgrind_output; then
        log_success "
        ✅ Test for $input finished successfully without memory leaks"
        ((test_succ += 1))
      else
        log_error "
        ❌ Test for $input failed due to memory leaks"
        cat $valgrind_output
        ((test_fail += 1))
      fi
    else
      log_success "
      ✅ Test for $input finished successfully"
      ((test_succ += 1))
    fi
  else
    log_error "
    ❌ Test failed"
    log_error "input : $input"
    log_error "return code : $return_code  expected code: $code"
    ((test_fail += 1))
  fi
}

while [[ $# -gt 0 ]]; do
  case "$1" in
  --valgrind)
    use_valgrind=true
    shift
    ;;
  --all)
    run_all=true
    shift
    ;;
  --noic)
    run_noic=true
    shift
    ;;
  --ic)
    run_ic=true
    shift
    ;;
  --merlin)
    IC_PATH="/pub/courses/ifj/ic24int/linux/ic24int"
    shift
    ;;
  *)
    echo "Unknown flag: $1"
    exit 1
    ;;
  esac
done

touch $valgrind_output
run_make
run_tests
rm -f $valgrind_output
rm -f prg.code
rm -f prg.out
