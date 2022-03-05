import os
import sys
from chardet.universaldetector import UniversalDetector
from filehandle.get_file_list import *
from filehandle.read_write_operator import read_binary_file, write_binary_file


def get_encode_info(file):
    with open(file, 'rb') as f:
        detector = UniversalDetector()
        for line in f.readlines():
            detector.feed(line)
            if detector.done:
                break
        detector.close()
        return detector.result['encoding']


def convert_encode2utf8(file, original_encode, des_encode):
    try:
        file_content = read_binary_file(file)
        file_decode = file_content.decode(original_encode, 'ignore')
        file_encode = file_decode.encode(des_encode)
        write_binary_file(file_encode, file)
    except TypeError as e:
        print("{} raise error {}".format(file, e))


def covert2utf8(Filelist):
    for filename in Filelist:
        file_content = read_binary_file(filename)
        encode_info = get_encode_info(filename)
        if encode_info != 'utf-8':
            convert_encode2utf8(filename, encode_info, 'utf-8')
        encode_info = get_encode_info(filename)


def main(argv):
    if len(argv) == 1:
        print("No input src directory, exiting")
        sys.exit(-1)

    if len(argv) == 3:
        print("Too many input manifests, exiting")
        sys.exit(-1)

    path = argv[1]
    file_types = ["*.h", "*.c",  "*.cpp", "*.cs"]   # available to add more file type into this list
    for file_type in file_types:
        file_list = get_file_list(path, file_type)
        covert2utf8(file_list)


if __name__ == "__main__":
    main(sys.argv)
