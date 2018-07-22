#!/usr/bin/python2.7
import os, sys, shutil, platform, time
import cembed

OUTPUT = "bin/nest"
EMBED_DIR = "src/embed"
TEMPSRC_DIR = ".tempsrc"
COMPILER = "clang"
INCLUDE_PATH = [
  TEMPSRC_DIR,
  "wren/src/optional",
  "wren/src/include",
  "wren/src/vm",
  "src/lib",
  "src",
]
SOURCE = [
  "wren/src/optional",
  "wren/src/vm",
  "src",
]
FLAGS = [ "-Wall", "-Wextra", "--std=c99", "-fno-strict-aliasing", "-Wno-unused-parameter" ]
LINK = [ "m" ]
LINK_PATH = [  ]
LINK_FLAGS = [ ]
DEFINE = [  ]
EXTRA = [  ]

if platform.system() == "Windows":
  sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
  OUTPUT += ".exe"
  LINK += [ "mingw32", "ws2_32" ]

def fmt(fmt, dic):
  for k in dic:
    fmt = fmt.replace("{" + k + "}", str(dic[k]))
  return fmt


def listFiles(directory):
  for root, subdir, sources in os.walk(directory):
    for source in sources:
      yield os.path.join(root, source)


def cleanup():
  if os.path.exists(TEMPSRC_DIR):
    shutil.rmtree(TEMPSRC_DIR)


def compile_object_file(verbose, source, output):
  cmd = fmt(
    "{compiler} -c -o {tempsrc}/{output} {flags} {include_path} {define} {source} " +
    "{extra}", {

    "compiler"     : COMPILER,
    "tempsrc"      : TEMPSRC_DIR,
    "output"       : output,
    "source"       : source,
    "include_path" : " ".join(map(lambda x:"-I" + x, INCLUDE_PATH)),
    "define"       : " ".join(map(lambda x:"-D" + x, DEFINE)),
    "flags"        : " ".join(FLAGS),
    "extra"        : " ".join(EXTRA),
  })

  if verbose:
    print cmd

  return os.system(cmd)

def compile_executable(verbose):
  cmd = fmt(
    "{compiler} -o {output} {flags} {tempsrc}/*.o {linker_path} {link}", {
    "compiler"     : COMPILER,
    "tempsrc"      : TEMPSRC_DIR,
    "output"       : OUTPUT,
    "flags"        : " ".join(LINK_FLAGS),
    "linker_path"  : " ".join(map(lambda x:"-L" + x, LINK_PATH)),
    "link"         : " ".join(map(lambda x:"-l" + x, LINK)),
  })

  if verbose:
    print cmd

  return os.system(cmd)


def main():
  global FLAGS, LINK_FLAGS, LINK, DEFINE

  print "initing..."

  if not os.path.exists("wren"):
    print "downloading dependencies..."
    res = os.system("sh install_deps.sh")
    if res != 0:
      sys.exit(res)
  
  starttime = time.time()

  # Handle args
  build = "release" if "release" in sys.argv else "debug"
  verbose = "verbose" in sys.argv

  # Handle build type
  if build == "debug":
    FLAGS += [ "-g"  ]
  else:
    FLAGS += [ "-O3", "-flto" ]
    LINK_FLAGS += [ "-flto" ]
    if platform.system() == "Windows":
      DEFINE += [ "NEST_RELEASE" ]

  print "building (" + build + ")..."

  # Make sure there arn't any temp files left over from a previous build
  cleanup()

  # Make sure the previous binary is deleted (windows)
  if os.path.isfile(OUTPUT):
    os.remove(OUTPUT)

  # Create directories
  os.makedirs(TEMPSRC_DIR)
  outdir = os.path.dirname(OUTPUT)
  if not os.path.exists(outdir):
    os.makedirs(outdir)

  # Create embedded-file header files
  for filename in os.listdir(EMBED_DIR):
    fullname = EMBED_DIR + "/" + filename
    res = cembed.process(fullname)
    open(TEMPSRC_DIR + "/" + cembed.safename(fullname) + ".h", "wb").write(res)

  # Build object files
  for directory in SOURCE:
    for filename in listFiles(directory):
      base, ext = os.path.splitext(os.path.basename(filename))
      if ext == ".c":
        res = compile_object_file(verbose, filename, base + ".o")
        if res != 0:
          sys.exit(res)

  res = compile_executable(verbose)
  if res != 0:
    sys.exit(res)

  if build == "release":
    print "stripping..."
    os.system("strip %s" % OUTPUT)

  print "cleaning up..."
  cleanup()

  print "done (%.2fs)" % (time.time() - starttime)

if __name__ == "__main__":
  main()