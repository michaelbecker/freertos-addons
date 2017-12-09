#!/usr/bin/python3
#############################################################################
#
# MIT License
# 
# Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
# 
# Permission is hereby granted, free of charge, to any person obtaining a 
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the 
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included 
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT 
# OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR 
# THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
#############################################################################
"""
Script to recursively update a set of files' headers.
"""
import os
import re
import sys
import argparse


def find_all_files(directory, filename_filter = None, dirname_filter = None, 
                    ignore_dot_files = True):
    """ Return a list of all normal files in the directory
        as well as all subdirs. This performs a breath first 
        search because, well, I've never programed one before.
        By default, we will ignore any file or directory that 
        starts with a dot, as per Unix convention.
    """
    
    # Normalize the initial directory.
    directory = os.path.realpath(directory)
    original_dir = os.getcwd()
    os.chdir(directory)
    dir_list = []
    file_list = []

    for e in os.listdir(directory):

        if ignore_dot_files and e[0] == ".":
            continue

        if os.path.isfile(e):
            e = os.path.realpath(e)
            if filename_filter == None:
                file_list.append(e)
            elif filename_filter(e):
                file_list.append(e)

        elif os.path.isdir(e):
            e = os.path.realpath(e)
            if dirname_filter == None:
                dir_list.append(e)
            elif dirname_filter(e):
                dir_list.append(e)

    for d in dir_list:
        file_list = file_list + find_all_files(d, filename_filter)

    os.chdir(original_dir)

    return file_list


def make_file_extension_match(extension):
    """Return a function that searches for filename extensions.
    """
    def is_file_match(filename):
        match = re.search("\." + extension + "$", filename)
        if match:
            return True
        else:
            return False

    return is_file_match


def make_filename_match(base_name):
    """Return a function that searches for a filename.
    """
    def is_file_match(filename):
        match = re.search(base_name + "$", filename)
        if match:
            return True
        else:
            return False

    return is_file_match


def read_file(filename):
    """Read a text file and return the contents as 
    a list of lines
    """
    f = open(filename, "r")
    try:
        lines = f.readlines()
    except:
        print ("Failed reading " + filename)
    f.close()
    return lines


def write_file(filename, lines):
    """Write a list of lines into a text file.
    """
    f = open(filename, "w")
    for l in lines:
        f.write(l)
    f.close()


def strip_header(lines, find_header_start, find_header_end, find_shebang = False):
    """ Remove the header lines from a list of file lines.
        We require that the header start on the _first_ line, 
        unless there is a shebang and we are looking for it, in 
        which case it may start on the second line if there is a 
        shebang.
    """
    search_for_header = True
    in_header = False
    new_lines = []

    for l in lines:
        
        # Run once, if ever. But if it's here we need to
        # look before we look for a header.
        if find_shebang:
            find_shebang = False
            # Using match is "like" using regex "^#!"
            match = re.match("#!", l)
            if match:
                new_lines.append(l)
                continue

        # Condition: The header has to start on the first line 
        # (or the second if there's a shebang line present).
        if search_for_header:
            search_for_header = False
            if find_header_start(l):
                in_header = True
                # We need to continue here. If we don't, for files
                # that have symetric comment delimiters we pick up
                # the end marker as the same line as the start.
                continue

        if not in_header:
            new_lines.append(l)

        if in_header:
            if find_header_end(l):
                in_header = False

    return new_lines


def insert_new_header(file_lines, new_header, find_shebang = False):
    """ Insert the list of new header lines in the file.
    """
    new_lines = []

    if find_shebang:
        find_shebang = False
        # Using match is "like" using regex "^#!"
        match = re.match("#!", file_lines[0])
        if match:
            new_lines.append(file_lines[0])
            new_lines = new_lines + new_header
            new_lines = file_lines[1:]
        else:
            new_lines = new_header + file_lines
    else:
        new_lines = new_header + file_lines

    return new_lines


def print_lines(lines):
    """ Debug utility, just print out the lines.
    """
    for l in lines:
        l = l.rstrip()
        print(l,)


def generate_c_header_finders():
    """ Generate the header finders for c style files.
    """
    def find_header_start(line):
        match = re.search("/\*\*", line)
        if match:
            return True
        else:
            return False

    def find_header_end(line):
        match = re.search("\*\*/", line)
        if match:
            return True
        else:
            return False

    return find_header_start, find_header_end


def generate_c_and_cpp_header_finders():
    """ Generate the header finders for c & c++ style files.
    """
    c_style = False
    cpp_style = False

    def find_header_start(line):
        nonlocal c_style, cpp_style

        match = re.search("/\*\*", line)
        if match:
            c_style = True
            return True
        
        match = re.search("///", line)
        if match:
            cpp_style = True
            return True
        else:
            return False

    def find_header_end(line):
        nonlocal c_style, cpp_style
        
        if c_style:
            match = re.search("\*\*/", line)
            if match:
                return True
            else:
                return False
        elif cpp_style:
            match = re.search("///", line)
            if match:
                return True
            else:
                return False
        else:
            return False

    return find_header_start, find_header_end


def update_all_cpp_files(start_dir, new_header):
    """ Handle c++ source files.
    """
    start, end = generate_c_and_cpp_header_finders()

    file_list = find_all_files(start_dir, make_file_extension_match("cpp"))
    for f in file_list:
        lines = read_file(f)
        lines = strip_header(lines, start, end)
        lines = insert_new_header(lines, new_header)
        write_file(f, lines)


def update_all_hpp_files(start_dir, new_header):
    """ Handle c++ header files.
    """
    start, end = generate_c_and_cpp_header_finders()

    file_list = find_all_files(start_dir, make_file_extension_match("hpp"))
    for f in file_list:
        lines = read_file(f)
        lines = strip_header(lines, start, end)
        lines = insert_new_header(lines, new_header)
        write_file(f, lines)


def update_all_c_files(start_dir, new_header):
    """ Handle c source files.
    """
    start, end = generate_c_header_finders()

    file_list = find_all_files(start_dir, make_file_extension_match("c"))
    for f in file_list:
        lines = read_file(f)
        lines = strip_header(lines, start, end)
        lines = insert_new_header(lines, new_header)
        write_file(f, lines)


def update_all_h_files(start_dir, new_header):
    """ Handle c header files.
    """
    start, end = generate_c_header_finders()

    file_list = find_all_files(start_dir, make_file_extension_match("h"))
    for f in file_list:
        lines = read_file(f)
        lines = strip_header(lines, start, end)
        lines = insert_new_header(lines, new_header)
        write_file(f, lines)


def update_all_makefiles(start_dir, new_header):
    """ Handle makefiles.
    """
    def find_header_start_and_end(line):
        match = re.match("##", line)
        if match:
            return True
        else:
            return False

    file_list = find_all_files(start_dir, make_filename_match("makefile"))
    for f in file_list:
        lines = read_file(f)
        lines = strip_header(lines, find_header_start_and_end, 
                find_header_start_and_end)
        lines = insert_new_header(lines, new_header)
        write_file(f, lines)


def debug_print_all(start_dir):
    """ Debugging, print all files / dirs found.
    """

    file_list = find_all_files(start_dir)
    for f in file_list:
        print(f)

# Script proper starts here.

parser = argparse.ArgumentParser()
parser.add_argument("header_file", help="File containing the new header.")
parser.add_argument("file_type", help="[c|h|cpp|hpp|makefile] File type to be updated.")
parser.add_argument("--dir", help="Directory to start scaning in. Default is current directory.")
args = parser.parse_args()


if not args.dir:
    start_dir = "."
else:
    start_dir = args.dir


header_lines = read_file(args.header_file)


if (args.file_type == "cpp"):
    update_all_cpp_files(start_dir, header_lines)

elif (args.file_type == "c"):
    update_all_c_files(start_dir, header_lines)

elif (args.file_type == "h"):
    update_all_h_files(start_dir, header_lines)

elif (args.file_type == "hpp"):
    update_all_hpp_files(start_dir, header_lines)

elif (args.file_type == "makefile"):
    update_all_makefiles(start_dir, header_lines)

elif (args.file_type == "debugprintall"):
    debug_print_all(start_dir)

else:
    print("Unsupported file type")
    sys.exit(0)


