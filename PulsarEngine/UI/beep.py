import os
import re

def process_files():
    """
    Gets all .axaml and .cs files in the current folder and subfolders,
    and writes their content to an output.txt file.
    """
    
    current_folder = os.getcwd()  # Get the current working directory

    all_files = []
    for root, _, files in os.walk(current_folder):
        for file in files:
            if file.endswith(".cpp") or file.endswith(".hpp"):
                filepath = os.path.join(root, file)
                all_files.append(filepath)

    try:
        with open("output.txt", "w", encoding="utf-8") as outfile:
            for filepath in all_files:
                try:
                    with open(filepath, "r", encoding="utf-8") as infile:
                        content = infile.read()
                        # Get the filename (without the full path)
                        filename = os.path.relpath(filepath, current_folder)
                        outfile.write(f"{filename}\n{content}\n\n")
                except UnicodeDecodeError as e:
                    print(f"Error decoding file {filepath}: {e}")
                    # Important: Skip the file and don't crash the whole process
                    continue
                except Exception as e:
                    print(f"Error processing file {filepath}: {e}")
                    # Important: Skip the file and don't crash the whole process
                    continue
    except Exception as e:
        print(f"Error writing to output file: {e}")


# Call the function directly
process_files()