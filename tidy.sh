#!/bin/sh

# tidy-cpp.sh
# A script to run clang-tidy on C++ files in include/ and src/ directories
# with automatic compilation database generation

set -e

# Check if we're in a terminal that supports colors
if test -t 1; then
  # Colors for terminal output
  RED='\033[0;31m'
  GREEN='\033[0;32m'
  YELLOW='\033[0;33m'
  BLUE='\033[0;34m'
  NC='\033[0m' # No Color
  
  # Function to safely print colored text
  print_color() {
    printf "%b%s%b\n" "$1" "$2" "$NC"
  }
else
  # No color support
  RED=""
  GREEN=""
  YELLOW=""
  BLUE=""
  NC=""
  
  # Function to print text without colors
  print_color() {
    printf "%s\n" "$2"
  }
fi

# Print help message
print_help() {
    printf "%bUsage:%b %s [OPTIONS]\n" "$BLUE" "$NC" "$0"
    echo "Run clang-tidy on C++ files in include/ and src/ directories."
    echo 
    printf "%bOptions:%b\n" "$BLUE" "$NC"
    echo "  -h, --help              Show this help message"
    echo "  -f, --fix               Apply suggested fixes automatically"
    echo "  -c, --checks CHECKS     Specify custom checks (default: bugprone-*,performance-*,readability-*)"
    echo "  -p, --path PATH         Path to compile_commands.json"
    echo "  -v, --verbose           Show verbose output"
    echo "  -g, --generate-compdb   Generate compilation database automatically (requires compile_commands.py)"
    echo "  -i, --include-dirs DIRS Additional include directories (comma-separated)"
}

# Check if clang-tidy is installed
CLANG_TIDY_PATH=$(command -v clang-tidy || echo "")
echo "$CLANG_TIDY_PATH"
if [ -z "$CLANG_TIDY_PATH" ]; then
    printf "%bError: clang-tidy is not installed.%b\n" "$RED" "$NC"
    echo "Please install clang-tidy first:"
    echo "  Ubuntu/Debian: sudo apt install clang-tidy"
    echo "  macOS: brew install llvm"
    exit 1
fi

# Default options
FIX_ERRORS=0
CHECKS="bugprone-*,performance-*,readability-*"
COMPILE_COMMANDS=""
VERBOSE=0
GENERATE_COMPDB=0
INCLUDE_DIRS=""

# Parse command line arguments
while [ $# -gt 0 ]; do
    case $1 in
        -h|--help)
            print_help
            exit 0
            ;;
        -f|--fix)
            FIX_ERRORS=1
            shift
            ;;
        -c|--checks)
            CHECKS="$2"
            shift 2
            ;;
        -p|--path)
            COMPILE_COMMANDS="-p $2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=1
            shift
            ;;
        -g|--generate-compdb)
            GENERATE_COMPDB=1
            shift
            ;;
        -i|--include-dirs)
            INCLUDE_DIRS="$2"
            shift 2
            ;;
        *)
            printf "%bUnknown option: %s%b\n" "$RED" "$1" "$NC"
            print_help
            exit 1
            ;;
    esac
done

# Check if directories exist
if [ ! -d "include" ] && [ ! -d "src" ]; then
    printf "%bError: Neither include/ nor src/ directories found.%b\n" "$RED" "$NC"
    echo "Please run this script from your project's root directory."
    exit 1
fi

# Generate compilation database if requested
if [ "$GENERATE_COMPDB" = "1" ]; then
    # Check if we have a compile_commands.py helper script
    if [ -f "./compile_commands.py" ]; then
        printf "%bGenerating compilation database...%b\n" "$BLUE" "$NC"
        python3 ./compile_commands.py
        
        if [ -f "./compile_commands.json" ]; then
            COMPILE_COMMANDS="-p ."
            printf "%bCompilation database generated successfully%b\n" "$GREEN" "$NC"
        else
            printf "%bFailed to generate compilation database%b\n" "$RED" "$NC"
        fi
    else
        # Create a simple compilation database for basic projects
        printf "%bGenerating basic compilation database...%b\n" "$BLUE" "$NC"
        
        # Create temporary header files list
        HEADERS=$(find include -type f \( -name "*.h" -o -name "*.hpp" \) 2>/dev/null | tr '\n' ' ')
        
        # Create temporary source files list
        SOURCES=$(find src -type f \( -name "*.cpp" -o -name "*.cc" \) 2>/dev/null | tr '\n' ' ')
        
        # Generate compilation database
        echo "[" > compile_commands.json
        
        # Add include directories
        INCLUDE_FLAGS=""
        if [ ! -z "$INCLUDE_DIRS" ]; then
            for dir in $(echo "$INCLUDE_DIRS" | tr ',' ' '); do
                INCLUDE_FLAGS="$INCLUDE_FLAGS -I$dir"
            done
        fi
        
        # Always include the standard include directory
        INCLUDE_FLAGS="$INCLUDE_FLAGS -Iinclude"
        
        # Add entry for each source file
        for source in $SOURCES; do
            echo "  {" >> compile_commands.json
            echo "    \"directory\": \"$(pwd)\"," >> compile_commands.json
            echo "    \"command\": \"clang++ -std=c++17 $INCLUDE_FLAGS -c $source\"," >> compile_commands.json
            echo "    \"file\": \"$source\"" >> compile_commands.json
            echo "  }," >> compile_commands.json
        done
        
        # Fix the last comma and close the array
        sed -i '$ s/,$//' compile_commands.json
        echo "]" >> compile_commands.json
        
        COMPILE_COMMANDS="-p ."
        printf "%bBasic compilation database generated%b\n" "$GREEN" "$NC"
    fi
fi

# Find all C++ files
CPP_FILES=$(find include src -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.h" -o -name "*.hpp" \) 2>/dev/null || echo "")

if [ -z "$CPP_FILES" ]; then
    printf "%bWarning: No C++ files found in include/ or src/ directories.%b\n" "$YELLOW" "$NC"
    exit 0
fi

if [ "$VERBOSE" = "1" ]; then
    printf "%bFound the following files to analyze:%b\n" "$BLUE" "$NC"
    echo "$CPP_FILES" | tr ' ' '\n'
    echo
fi

# Build clang-tidy command
CLANG_TIDY_CMD="$CLANG_TIDY_PATH -checks=$CHECKS"

if [ "$FIX_ERRORS" = "1" ]; then
    CLANG_TIDY_CMD="$CLANG_TIDY_CMD -fix"
fi

if [ ! -z "$COMPILE_COMMANDS" ]; then
    CLANG_TIDY_CMD="$CLANG_TIDY_CMD $COMPILE_COMMANDS"
fi

# If user didn't provide a compilation database and we didn't generate one,
# add header-filter to avoid "header file not found" errors
if [ -z "$COMPILE_COMMANDS" ]; then
    printf "%bWARNING: No compilation database provided. Header errors may occur.%b\n" "$YELLOW" "$NC"
    printf "%bTip: Use --generate-compdb to create a compilation database%b\n" "$BLUE" "$NC"
    echo "     or specify path with --path option."
    echo
    
    # Add include directory to command
    CLANG_TIDY_CMD="$CLANG_TIDY_CMD -- -Iinclude"
    
    # Add any additional include directories
    if [ ! -z "$INCLUDE_DIRS" ]; then
        for dir in $(echo "$INCLUDE_DIRS" | tr ',' ' '); do
            CLANG_TIDY_CMD="$CLANG_TIDY_CMD -I$dir"
        done
    fi
fi

# Count files for progress indication
TOTAL_FILES=$(echo "$CPP_FILES" | wc -w)
CURRENT_FILE=0
FAILED_FILES=0
SUCCESSFUL_FILES=0

printf "%bStarting C++ linting with clang-tidy...%b\n" "$GREEN" "$NC"
printf "Using checks: %b%s%b\n" "$YELLOW" "$CHECKS" "$NC"

# Process each file
for file in $CPP_FILES; do
    CURRENT_FILE=$((CURRENT_FILE + 1))
    printf "%b[%d/%d] Processing:%b %s\n" "$BLUE" "$CURRENT_FILE" "$TOTAL_FILES" "$NC" "$file"
    
    if [ "$VERBOSE" = "1" ]; then
        # Run with full output
        if $CLANG_TIDY_CMD "$file"; then
            SUCCESSFUL_FILES=$((SUCCESSFUL_FILES + 1))
            printf "%bSuccessfully processed %s%b\n" "$GREEN" "$file" "$NC"
        else
            FAILED_FILES=$((FAILED_FILES + 1))
            printf "%bFailed to process %s%b\n" "$RED" "$file" "$NC"
        fi
    else
        # Run with minimal output
        if $CLANG_TIDY_CMD "$file" 2>&1 | grep -E "error:|warning:" > /dev/null; then
            FAILED_FILES=$((FAILED_FILES + 1))
            printf "%bIssues found in %s%b\n" "$RED" "$file" "$NC"
            # Show errors/warnings even in non-verbose mode
            $CLANG_TIDY_CMD "$file" 2>&1 | grep -E "error:|warning:"
        else
            SUCCESSFUL_FILES=$((SUCCESSFUL_FILES + 1))
            printf "%bNo issues in %s%b\n" "$GREEN" "$file" "$NC"
        fi
    fi
done

echo
printf "%bLinting completed!%b\n" "$GREEN" "$NC"
printf "Processed %b%d%b files:\n" "$BLUE" "$TOTAL_FILES" "$NC"
printf "  %b%d%b files with no issues\n" "$GREEN" "$SUCCESSFUL_FILES" "$NC"
printf "  %b%d%b files with issues\n" "$RED" "$FAILED_FILES" "$NC"

if [ $FAILED_FILES -gt 0 ]; then
    echo
    printf "To see detailed error information, run with %b--verbose%b flag\n" "$YELLOW" "$NC"
    if [ "$FIX_ERRORS" = "0" ]; then
        printf "To automatically fix issues where possible, run with %b--fix%b flag\n" "$YELLOW" "$NC"
    fi
    # If header file not found errors were common
    if $CLANG_TIDY_CMD "$file" 2>&1 | grep -E "file not found" > /dev/null; then
        echo
        printf "%bIt looks like you have 'file not found' errors.%b\n" "$YELLOW" "$NC"
        echo "This usually happens when clang-tidy doesn't know how to compile your project."
        echo "Try one of these solutions:"
        echo "  1. Run with --generate-compdb to create a basic compilation database"
        echo "  2. Specify include directories with --include-dirs dir1,dir2,..."
        echo "  3. Use CMake to generate a proper compilation database:"
        echo "     $ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ."
    fi
    exit 1
else
    exit 0
fi
