#!/usr/bin/python3

import csv
import urllib.request
import urllib.parse
import json
import os
import os.path

INPUT_FILE = "404-errors.csv" # Screaming Frog CSV export of 404 errors
OUTPUT_FILE = "known.csv" # the output file
FILE_PATH = "files/" # must end in path separator

# returns a unique file name in the path, based on the given URL
def pick_local_file_name(url, filepath):
    clean = url.split("/")[-1] # get filename from URL
    clean = "".join(x for x in clean if x.isalnum() or x in "-_.")
    if not "." in clean: clean += ".bin" # placeholder extension
    # swap out "." in file name, lower-case extension, max 50 characters
    clean = "-".join(clean.split(".")[:-1])[:50] + "." + clean.split(".")[-1].lower()

    # find a file name that's unique
    for counter in range(0, 100):
        if counter > 0:
            fn = "archived_" + clean.split(".")[0] + "-" + str(counter) + "." + clean.split(".")[1]
        else:
            fn = "archived_" + clean
        if not os.path.isfile(filepath + fn): break

    return fn


# downloads the artists, um, image previously known as url
def download_image(archive_url, file_path):
    print("Archive copy at " + archive_url)
    image_url = archive_url.replace("/http", "if_/http")
    print("Image probably at " + image_url)
    filename = pick_local_file_name(image_url, file_path)
    print("Saving to " + filename)
    fp = urllib.request.urlopen(image_url)
    img_bytes = fp.read()
    if filename.endswith(".bin"):
        if (b"PNG" in img_bytes[:50]):
            filename = filename.replace(".bin", ".png")
        elif (b"GIF" in img_bytes[:50]):
            filename = filename.replace(".bin", ".gjf")
        elif (b"JFIF" in img_bytes[:50]):
            filename = filename.replace(".bin", ".jpg")
    open(file_path + filename, "wb").write(img_bytes)
    fp.close()
    return filename


# checks archive.org API for archived copies
def check_archive_org(url):
    fetch_url = "https://archive.org/wayback/available?url=" + url
    print("Checking API " + fetch_url)
    fp = urllib.request.urlopen(fetch_url)
    mybytes = fp.read()
    mystr = mybytes.decode("utf8")
    fp.close()
    print(mystr)
    result_obj = json.loads(mystr)
    return result_obj


# check archive.org, download image to local store
def fetch_file(url, file_path):
    ob = check_archive_org(url)
    if ('archived_snapshots' in ob) and ('closest' in ob['archived_snapshots']):
        filename = download_image(ob['archived_snapshots']['closest']['url'], file_path)
    else:
        nofile_url = "/".join(url.split("/")[:-1]) + "/*"
        ob = check_archive_org(nofile_url)
        if ('archived_snapshots' in ob) and ('closest' in ob['archived_snapshots']):
            filename = download_image(ob['archived_snapshots']['closest']['url'], file_path)
        else:
            filename = False
            print(".. missing ..")
    return filename


def main():
    # get list of 404 errors from Screaming Frog
    full_list = []
    with open(INPUT_FILE, "r") as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader: full_list.append(row)

    # get list so far, if any
    known_list = {} # key by URL
    if os.path.isfile(OUTPUT_FILE):
        with open(OUTPUT_FILE, "r") as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader: known_list[row["Address"]] = row

    # create output directory if needed
    if not os.path.isdir(FILE_PATH): os.mkdir(FILE_PATH)

    # process list
    max_items = 1000000 # set to a high number if you want all, or a low number to try out
    for row in full_list:
        url = row["Address"]
        print("Checking: " + url)
        if ((("bp.blogspot." in url) or (".googleusercontent.com" in url) or ("image" in row["Content Type"])) 
                and (url not in known_list)):
            local_file_name = fetch_file(url, FILE_PATH)
            print("")
            if local_file_name:
                item = {"Address": url, "LocalFile": local_file_name}
                known_list[url] = item
        max_items -= 1
        if max_items < 1: break

    # save progress
    with open(OUTPUT_FILE, "w") as csvfile:
        fieldnames = ["Address", "LocalFile"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for url in known_list.keys(): writer.writerow(known_list[url])

    print("Done.")

main()

