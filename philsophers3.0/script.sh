#!/bin/bash

# Philosophers Testing Automation Script
# This script automates the process of running multiple test executions
# and collecting their output in organized log files

# Configuration variables - modify these to customize your testing
PHILO_EXECUTABLE="./philo"
TESTS_DIR="tests"
DEFAULT_RUNS=5

# Color codes for better output readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to create tests directory if it doesn't exist
setup_test_environment() {
    if [ ! -d "$TESTS_DIR" ]; then
        print_status "Creating tests directory: $TESTS_DIR"
        mkdir -p "$TESTS_DIR"
    fi
}

# Function to check if the philo executable exists
check_executable() {
    if [ ! -f "$PHILO_EXECUTABLE" ]; then
        print_error "Executable '$PHILO_EXECUTABLE' not found!"
        print_status "Please compile your program first using 'make'"
        exit 1
    fi
    
    if [ ! -x "$PHILO_EXECUTABLE" ]; then
        print_error "File '$PHILO_EXECUTABLE' is not executable!"
        print_status "Run 'chmod +x $PHILO_EXECUTABLE' to make it executable"
        exit 1
    fi
}

# Function to run a single test configuration multiple times
run_test_series() {
    local test_name="$1"
    local num_runs="$2"
    local args="$3"
    local output_file="$TESTS_DIR/${test_name}.txt"
    
    print_status "Running test series: $test_name"
    print_status "Arguments: $args"
    print_status "Number of runs: $num_runs"
    print_status "Output file: $output_file"
    
    # Clear the output file and add header
    echo "=== Philosophers Test Results ===" > "$output_file"
    echo "Test: $test_name" >> "$output_file"
    echo "Arguments: $args" >> "$output_file"
    echo "Number of runs: $num_runs" >> "$output_file"
    echo "Timestamp: $(date)" >> "$output_file"
    echo "===============================" >> "$output_file"
    echo "" >> "$output_file"
    
    # Run the test multiple times
    for ((i=1; i<=num_runs; i++)); do
        echo "--- Test Run $i ---" >> "$output_file"
        echo "Started at: $(date '+%Y-%m-%d %H:%M:%S.%3N')" >> "$output_file"
        echo "" >> "$output_file"
        
        # Execute the program and capture output
        # Using timeout to prevent hanging tests
        timeout 10s $PHILO_EXECUTABLE $args >> "$output_file" 2>&1
        local exit_code=$?
        
        echo "" >> "$output_file"
        echo "Exit code: $exit_code" >> "$output_file"
        echo "Finished at: $(date '+%Y-%m-%d %H:%M:%S.%3N')" >> "$output_file"
        echo "" >> "$output_file"
        
        # Provide progress feedback
        if [ $exit_code -eq 124 ]; then
            print_warning "Test run $i timed out (>10s)"
        elif [ $exit_code -eq 0 ]; then
            print_success "Test run $i completed successfully"
        else
            print_warning "Test run $i exited with code $exit_code"
        fi
    done
    
    print_success "Test series '$test_name' completed. Results saved to $output_file"
}

# Function to display usage information
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -n, --runs NUM          Number of test runs per configuration (default: $DEFAULT_RUNS)"
    echo "  -d, --dir DIRECTORY     Tests output directory (default: $TESTS_DIR)"
    echo "  -e, --executable PATH   Path to philo executable (default: $PHILO_EXECUTABLE)"
    echo "  -c, --custom            Run custom test configuration"
    echo "  -a, --all               Run all predefined test configurations"
    echo ""
    echo "Examples:"
    echo "  $0 -a                   # Run all tests with default settings"
    echo "  $0 -n 10 -a             # Run all tests 10 times each"
    echo "  $0 -c                   # Interactive mode for custom tests"
    echo "  $0 -n 3 -c              # Custom test with 3 runs"
}

# Function to run predefined test configurations
run_predefined_tests() {
    local runs="$1"
    
    print_status "Running predefined test configurations with $runs runs each"
    
    # Test configurations based on the correction requirements
    declare -a test_configs=(
        "single_philo_death:1 800 200 200"
        "basic_survival:5 800 200 200"
        "meal_count_termination:5 800 200 200 7"
        "tight_timing_survive:4 410 200 200"
        "tight_timing_die:4 310 200 100"
        "two_philo_critical:2 410 200 200"
        "stress_test:20 800 200 200"
        "quick_death_detection:3 200 100 100"
        "minimum_timing:4 60 60 60"
        "fast_eating_cycle:6 1000 100 100"
    )
    
    for config in "${test_configs[@]}"; do
        IFS=':' read -r test_name args <<< "$config"
        run_test_series "$test_name" "$runs" "$args"
        echo "" # Add spacing between test series
    done
}

# Function to run custom test configuration
run_custom_test() {
    local runs="$1"
    
    echo ""
    print_status "=== Custom Test Configuration ==="
    
    # Get test name from user
    echo -n "Enter test name (no spaces): "
    read -r test_name
    
    if [ -z "$test_name" ]; then
        print_error "Test name cannot be empty!"
        return 1
    fi
    
    # Get program arguments from user
    echo -n "Enter program arguments (e.g., '5 800 200 200'): "
    read -r args
    
    if [ -z "$args" ]; then
        print_error "Arguments cannot be empty!"
        return 1
    fi
    
    # Validate arguments format (basic check)
    if ! echo "$args" | grep -E '^[0-9]+( [0-9]+)*$' > /dev/null; then
        print_warning "Arguments should be space-separated numbers"
        echo -n "Continue anyway? (y/N): "
        read -r confirm
        if [[ ! "$confirm" =~ ^[Yy]$ ]]; then
            print_status "Test cancelled"
            return 1
        fi
    fi
    
    # Run the custom test
    run_test_series "$test_name" "$runs" "$args"
}

# Function to analyze test results
analyze_results() {
    if [ ! -d "$TESTS_DIR" ] || [ -z "$(ls -A "$TESTS_DIR")" ]; then
        print_warning "No test results found in $TESTS_DIR"
        return
    fi
    
    print_status "=== Test Results Analysis ==="
    for file in "$TESTS_DIR"/*.txt; do
        if [ -f "$file" ]; then
            local filename=$(basename "$file")
            local death_count=$(grep -c "died" "$file" 2>/dev/null || echo "0")
            local timeout_count=$(grep -c "Exit code: 124" "$file" 2>/dev/null || echo "0")
            local success_count=$(grep -c "Exit code: 0" "$file" 2>/dev/null || echo "0")
            
            echo "File: $filename"
            echo "  Deaths detected: $death_count"
            echo "  Timeouts: $timeout_count"
            echo "  Successful runs: $success_count"
            echo ""
        fi
    done
}

# Main script execution
main() {
    local runs="$DEFAULT_RUNS"
    local run_all=false
    local run_custom=false
    local show_analysis=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -n|--runs)
                if [[ $2 =~ ^[0-9]+$ ]] && [ "$2" -gt 0 ]; then
                    runs="$2"
                    shift 2
                else
                    print_error "Invalid number of runs: $2"
                    exit 1
                fi
                ;;
            -d|--dir)
                TESTS_DIR="$2"
                shift 2
                ;;
            -e|--executable)
                PHILO_EXECUTABLE="$2"
                shift 2
                ;;
            -a|--all)
                run_all=true
                shift
                ;;
            -c|--custom)
                run_custom=true
                shift
                ;;
            --analyze)
                show_analysis=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # If no specific action is requested, show usage
    if [ "$run_all" = false ] && [ "$run_custom" = false ] && [ "$show_analysis" = false ]; then
        show_usage
        exit 0
    fi
    
    # Setup environment
    setup_test_environment
    check_executable
    
    # Execute requested actions
    if [ "$run_all" = true ]; then
        run_predefined_tests "$runs"
    fi
    
    if [ "$run_custom" = true ]; then
        run_custom_test "$runs"
    fi
    
    if [ "$show_analysis" = true ]; then
        analyze_results
    fi
    
    # Always show brief analysis after running tests
    if [ "$run_all" = true ] || [ "$run_custom" = true ]; then
        echo ""
        analyze_results
    fi
    
    print_success "Testing automation completed!"
}

# Run the main function with all arguments
main "$@"
